$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'storage-timeout-trace-patch.ps1')
$root = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$os = [IO.File]::ReadAllBytes((Join-Path $root 'build\smoke\ChisBurner-storage-stream-0.1.0-smoke.42-crc-control\SD_ROOT\Assets\chisburner\common\os.bin'))
$instructions = [IO.File]::ReadAllBytes((Join-Path $root 'build\storage-trace-patch\patch.bin'))
$patched = Set-ChisBurnerTimeoutTracePatch -Bytes $os -Instructions $instructions
if ($patched.Length -ne $os.Length) { throw 'Image layout changed' }
$null = Assert-ChisBurnerOsIntegrity -Bytes $patched
# The whole startup region and all bytes before the first timeout print stay
# identical; the patch utility checks every other byte against its allowlist.
for ($i=0; $i -lt 0x933C; ++$i) {
    if ($patched[$i] -ne $os[$i]) { throw 'Startup/poll code changed' }
}
$bad = [byte[]]$os.Clone()
$bad[0] = $bad[0] -bxor 1
$rejected = $false
try { $null = Set-ChisBurnerTimeoutTracePatch -Bytes $bad -Instructions $instructions }
catch { $rejected = $true }
if (-not $rejected) { throw 'Wrong base OS accepted' }
$badInstructions = [byte[]]$instructions.Clone()
$badInstructions[2] = 0
$rejected = $false
try { $null = Set-ChisBurnerTimeoutTracePatch -Bytes $os -Instructions $badInstructions }
catch { $rejected = $true }
if (-not $rejected) { throw 'Wrong patch instructions accepted' }
$d = & (Join-Path $PSScriptRoot 'decode-storage-trace.ps1') DF3C1C60
if (-not $d.BridgeLaunchedFlush -or $d.HostReadCommand -or
    -not $d.CpuCommandActive -or $d.CpuAckInput -or $d.PublicStatus -ne '60') {
    throw 'No-host-poll trace decode mismatch'
}
$d = & (Join-Path $PSScriptRoot 'decode-storage-trace.ps1') D06FF3C3
if (-not $d.HostWroteComplete -or -not $d.CpuAckInput -or
    -not $d.CpuDoneInput -or $d.CpuCommandActive) { throw 'Completion trace decode mismatch' }
Write-Output 'PASS: exact-base guard, instruction guard, unchanged layout/poll loops, repaired internal CRC, trace decoder'
