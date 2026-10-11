. (Join-Path $PSScriptRoot 'os-image-integrity.ps1')
function Set-ChisBurnerEventServicePatch {
    param([byte[]]$Bytes, [byte[]]$Instructions)
    $sha = [Security.Cryptography.SHA256]::Create()
    try { $hash = [BitConverter]::ToString($sha.ComputeHash($Bytes)).Replace('-','').ToLowerInvariant() }
    finally { $sha.Dispose() }
    if ($hash -ne '45792a4fe6937403fd73a5efd1d4077f65e6c62454aa1100f851c9c2b80d4e1a') { throw 'Requires exact smoke.44 OS' }
    $null = Assert-ChisBurnerOsIntegrity -Bytes $Bytes
    if ([BitConverter]::ToUInt32($Bytes,0x16B3C) -ne 0x01176B63u -or
        [BitConverter]::ToUInt32($Bytes,0xF080) -ne 0x5000806Fu) { throw 'Hook/tail-jump signature mismatch' }
    for ($off=0xF084; $off -lt 0xF0C0; $off+=4) {
        if ([BitConverter]::ToUInt32($Bytes,$off) -ne 0x13u) { throw 'Code cave is not expected alignment padding' }
    }
    if ($Instructions.Length -ne 0x16B56) { throw 'Unexpected patch length' }
    $result = [byte[]]$Bytes.Clone()
    [Array]::Copy($Instructions,0xF084,$result,0xF084,56)
    [Array]::Copy($Instructions,0x16B3C,$result,0x16B3C,4)
    $result = Set-ChisBurnerOsCrc -Bytes $result
    for ($i=0; $i -lt $Bytes.Length-4; ++$i) {
        if ($Bytes[$i] -ne $result[$i] -and -not
            (($i -ge 0xF084 -and $i -lt 0xF0BC) -or ($i -ge 0x16B3C -and $i -lt 0x16B40))) {
            throw 'Out of scope OS change'
        }
    }
    return ,$result
}
