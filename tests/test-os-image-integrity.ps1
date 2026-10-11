[CmdletBinding()]
param()
$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'os-image-integrity.ps1')

function Expect-Failure([scriptblock]$Action, [string]$Name) {
    $failed = $false
    try { $null = & $Action } catch { $failed = $true }
    if (-not $failed) { throw "Expected validation failure: $Name" }
    Write-Output "PASS: $Name rejected"
}

$vector = [System.Text.Encoding]::ASCII.GetBytes('123456789')
if ([ChisBurner.Build.OsCrc32]::Compute($vector, $vector.Length) -ne 0xCBF43926u) {
    throw 'CRC-32/ISO-HDLC standard test vector failed'
}
if ([ChisBurner.Build.OsCrc32]::Compute([byte[]]@(), 0) -ne 0) {
    throw 'Empty CRC test vector failed'
}
Write-Output 'PASS: standard CRC vectors'

$root = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$smokeRoot = Join-Path $root 'build\smoke'
$baseline = Join-Path $smokeRoot ('ChisBurner-storage-stream-' +
    '0.1.0-smoke.31-flush-level\SD_ROOT\Assets\chisburner\common\os.bin')
$brokenBundle = Join-Path $smokeRoot ('ChisBurner-storage-stream-' +
    '0.1.0-smoke.41-size-commit')
$brokenPath = Join-Path $brokenBundle 'SD_ROOT\Assets\chisburner\common\os.bin'
$baselineBytes = [System.IO.File]::ReadAllBytes($baseline)
$brokenBytes = [System.IO.File]::ReadAllBytes($brokenPath)
$baselineInfo = Assert-ChisBurnerOsIntegrity -Bytes $baselineBytes
if ($baselineInfo.ComputedCrc -ne 0xD50797D4u) { throw 'Unexpected baseline CRC' }
Write-Output 'PASS: smoke.31 internal CRC'

Expect-Failure { Assert-ChisBurnerOsIntegrity -Bytes $brokenBytes } 'smoke.41 stale CRC'
Expect-Failure { Assert-ChisBurnerZipOsIntegrity -ZipPath "$brokenBundle.zip" } 'delivered smoke.41 ZIP'
Expect-Failure { Get-ChisBurnerOsIntegrity -Bytes ([byte[]](1, 2, 3)) } 'truncated image'

$badMagic = [byte[]]$baselineBytes.Clone()
$badMagic[$badMagic.Length - 8] = 0
Expect-Failure { Get-ChisBurnerOsIntegrity -Bytes $badMagic } 'damaged trailer magic'
$badPayload = [byte[]]$baselineBytes.Clone()
$badPayload[100] = $badPayload[100] -bxor 1
Expect-Failure { Assert-ChisBurnerOsIntegrity -Bytes $badPayload } 'payload bit flip'

$repaired = Set-ChisBurnerOsCrc -Bytes $brokenBytes
$info = Assert-ChisBurnerOsIntegrity -Bytes $repaired
if ($info.ComputedCrc -ne 0xEBB2C0DCu) { throw 'Unexpected repaired CRC' }
if ($repaired.Length -ne $brokenBytes.Length) { throw 'Repair changed length' }
$changed = 0
for ($i = 0; $i -lt $repaired.Length; ++$i) {
    if ($repaired[$i] -ne $brokenBytes[$i]) {
        if ($i -lt $info.CrcOffset) { throw 'Repair changed payload or metadata' }
        ++$changed
    }
}
if ($changed -ne 4) { throw 'Expected exactly four corrected bytes' }
$again = Set-ChisBurnerOsCrc -Bytes $repaired
if ([Convert]::ToBase64String($again) -cne [Convert]::ToBase64String($repaired)) {
    throw 'CRC repair is not idempotent'
}
if ((Get-ChisBurnerOsIntegrity -Bytes $brokenBytes).CrcMatches) {
    throw 'CRC repair mutated its source input'
}
Write-Output 'PASS: repair changes only four trailer bytes and is idempotent'
Write-Output 'OS image integrity regression: PASS'
