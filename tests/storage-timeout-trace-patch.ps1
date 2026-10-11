. (Join-Path $PSScriptRoot 'os-image-integrity.ps1')

function Set-ChisBurnerTimeoutTracePatch {
    param([Parameter(Mandatory)][byte[]]$Bytes,
          [Parameter(Mandatory)][byte[]]$Instructions)
    $sha = [Security.Cryptography.SHA256]::Create()
    try { $hash = [BitConverter]::ToString($sha.ComputeHash($Bytes)).Replace('-', '').ToLowerInvariant() }
    finally { $sha.Dispose() }
    if ($hash -ne 'b627365bef8b297e5362bd00863c57cfc0507459aebf5d5379f1cd23c26d24e8') {
        throw 'Timeout trace patch requires the exact smoke.42 OS'
    }
    $null = Assert-ChisBurnerOsIntegrity -Bytes $Bytes
    # Validate assembler output independently. This never changes the poll
    # loop: only the two error-print paths use DS_TRACE instead of DS_STATUS.
    if ($Instructions.Length -ne 8 -or
        [BitConverter]::ToUInt32($Instructions, 0) -ne 0x1B49A603u -or
        [BitConverter]::ToUInt32($Instructions, 4) -ne 0x00000013u) {
        throw 'Unexpected trace-print patch instructions'
    }
    $result = [byte[]]$Bytes.Clone()
    $regions = @(
        @{ Offset=0x933C; Original=0x0009A603u; From=0 },
        @{ Offset=0x934C; Original=0x03F67613u; From=4 },
        @{ Offset=0x9356; Original=0x0009A603u; From=0 },
        @{ Offset=0x9366; Original=0x03F67613u; From=4 }
    )
    foreach ($region in $regions) {
        if ([BitConverter]::ToUInt32($Bytes, $region.Offset) -ne $region.Original) {
            throw 'Error-print instruction signature mismatch'
        }
        [Array]::Copy($Instructions, $region.From, $result, $region.Offset, 4)
    }
    $result = Set-ChisBurnerOsCrc -Bytes $result
    for ($i=0; $i -lt $Bytes.Length - 4; ++$i) {
        if ($result[$i] -ne $Bytes[$i]) {
            $inScope = $false
            foreach ($region in $regions) {
                if ($i -ge $region.Offset -and $i -lt $region.Offset + 4) { $inScope = $true }
            }
            if (-not $inScope) { throw 'Out-of-scope OS change' }
        }
    }
    return ,$result
}
