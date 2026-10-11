[CmdletBinding()]
param([string]$Version = '0.1.0-smoke.42-crc-control')
$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'os-image-integrity.ps1')

$projectRoot = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$buildRoot = Join-Path $projectRoot 'build\smoke'
$baseRoot = Join-Path $buildRoot 'ChisBurner-storage-stream-0.1.0-smoke.41-size-commit'
$baseSd = Join-Path $baseRoot 'SD_ROOT'
$osRelative = 'Assets\chisburner\common\os.bin'
$critical = @{
    $osRelative = '4744ef44d1a1241d5bcec0a525957a76469f0bb8a5b57a810607042a232ddd1b'
    'Assets\chisburner\common\chisburnersmoke.elf' = '344907d6871f48f92d8ea93e97bfc835386d65ec0de57688f5f5809488d353f9'
    'Cores\ChisBurner.ChisBurner\chis.rbf_r' = 'b3c3d8b013859b555dce81cf556c342b0ab8ccf460418d9b4b32f1642784dd4b'
}
foreach ($relative in $critical.Keys) {
    $actual = (Get-FileHash -LiteralPath (Join-Path $baseSd $relative) -Algorithm SHA256).Hash.ToLowerInvariant()
    if ($actual -ne $critical[$relative]) { throw "Source hash mismatch: $relative" }
}

$sourceOs = [System.IO.File]::ReadAllBytes((Join-Path $baseSd $osRelative))
$oldInfo = Get-ChisBurnerOsIntegrity -Bytes $sourceOs
if ($oldInfo.StoredCrc -ne 0xD50797D4u -or $oldInfo.ComputedCrc -ne 0xEBB2C0DCu) {
    throw 'Source OS does not match the diagnosed smoke.41 CRC defect'
}
$correctedOs = Set-ChisBurnerOsCrc -Bytes $sourceOs
$newInfo = Assert-ChisBurnerOsIntegrity -Bytes $correctedOs
$changed = @()
for ($i = 0; $i -lt $sourceOs.Length; ++$i) {
    if ($sourceOs[$i] -ne $correctedOs[$i]) {
        if ($i -lt $newInfo.CrcOffset) { throw 'CRC control changed OS payload or metadata' }
        $changed += $i
    }
}
if ($correctedOs.Length -ne $sourceOs.Length -or $changed.Count -ne 4) {
    throw 'CRC-only control must change exactly four bytes without changing length'
}

if ($Version -notmatch '^0\.1\.0-smoke\.42-[a-z0-9-]+$' -or $Version.Length -gt 31) {
    throw 'Invalid smoke.42 package version'
}
$bundleRoot = Join-Path $buildRoot "ChisBurner-storage-stream-$Version"
$zipPath = "$bundleRoot.zip"
# Never replace historical evidence or a previously delivered package.
if ((Test-Path -LiteralPath $bundleRoot) -or (Test-Path -LiteralPath $zipPath)) {
    throw "Output already exists; choose a new smoke.42 version: $bundleRoot"
}
New-Item -ItemType Directory -Path $bundleRoot | Out-Null
Copy-Item -LiteralPath $baseSd -Destination $bundleRoot -Recurse
$sdRoot = Join-Path $bundleRoot 'SD_ROOT'
[System.IO.File]::WriteAllBytes((Join-Path $sdRoot $osRelative), $correctedOs)
$coreRoot = Join-Path $sdRoot 'Cores\ChisBurner.ChisBurner'
$instanceRoot = Join-Path $sdRoot 'Assets\chisburner\ChisBurner.ChisBurner'
Move-Item -LiteralPath (Join-Path $instanceRoot 'TF Storage SIZE 41.json') `
    -Destination (Join-Path $instanceRoot 'TF Storage CRC 42.json')
$coreJsonPath = Join-Path $coreRoot 'core.json'
$core = Get-Content -Raw -LiteralPath $coreJsonPath | ConvertFrom-Json
$core.core.metadata.version = $Version
$core.core.metadata.date_release = '2026-09-03'
[System.IO.File]::WriteAllText($coreJsonPath, ($core | ConvertTo-Json -Depth 20),
    (New-Object System.Text.UTF8Encoding($false)))

foreach ($relative in $critical.Keys) {
    if ($relative -eq $osRelative) { continue }
    $actual = (Get-FileHash -LiteralPath (Join-Path $sdRoot $relative) -Algorithm SHA256).Hash.ToLowerInvariant()
    if ($actual -ne $critical[$relative]) { throw "Unintended executable change: $relative" }
}
$null = Assert-ChisBurnerOsIntegrity -Bytes ([System.IO.File]::ReadAllBytes((Join-Path $sdRoot $osRelative)))
$osHash = (Get-FileHash -LiteralPath (Join-Path $sdRoot $osRelative) -Algorithm SHA256).Hash.ToLowerInvariant()
$readme = @"
ChisBurner - smoke.42 CRC-only control

Keep the cartridge slot EMPTY. Merge the complete SD_ROOT into the SD root,
including Saves, and approve replacement. Select "TF Storage CRC 42".
The application intentionally still says "TF STORAGE smoke.41": its ELF is
unchanged to preserve a strict CRC-only control. This is NOT an old-app warning.

Relative to smoke.41, only the last four bytes of os.bin change:
  stored internal CRC: D50797D4 -> EBB2C0DC
OS length, code, metadata, RBF and application are unchanged. No source rebuild.
The package retains smoke.41's diagnostic output-size patch unchanged; this is
not confirmation of that patch's correctness or of FLUSH recovery.

Primary expected result: no "OS CRC retry" or "BOOTING UNVERIFIED IMAGE".
FLUSH may still report ACK timeout / RC=-12. Report startup warnings and the
final FLUSH result separately. The package must not be treated as dump-ready.
The test recreates Saves/chisburner/common/storage-smoke.41.bin, a diagnostic file.

If "TF Storage CRC 42" is absent, the new instance was not merged. If CRC retry
persists, verify all replacements before interpreting it as a runtime fault.
"@
$provenance = @"
version=$Version
variant=smoke41_crc_only_control
base_os_sha256=$($critical[$osRelative])
os_sha256=$osHash
rbf_sha256=$($critical['Cores\ChisBurner.ChisBurner\chis.rbf_r'])
app_sha256=$($critical['Assets\chisburner\common\chisburnersmoke.elf'])
os_length=$($newInfo.Length)
internal_crc_before=$('{0:x8}' -f $oldInfo.StoredCrc)
internal_crc_after=$('{0:x8}' -f $newInfo.ComputedCrc)
changed_byte_count=4
changed_region=0x21904-0x21907
os_payload_and_metadata_byte_identical_smoke41=PASS
app_and_rbf_byte_identical_smoke41=PASS
source_rebuild=NO
"@
$utf8 = New-Object System.Text.UTF8Encoding($false)
[System.IO.File]::WriteAllText((Join-Path $bundleRoot 'README_TEST.txt'), $readme, $utf8)
[System.IO.File]::WriteAllText((Join-Path $bundleRoot 'BUILD_PROVENANCE.txt'), $provenance, $utf8)
$prefix = [System.IO.Path]::GetFullPath($sdRoot).TrimEnd('\') + '\'
$sums = Get-ChildItem -LiteralPath $sdRoot -Recurse -File | Sort-Object FullName | ForEach-Object {
    $relative = $_.FullName.Substring($prefix.Length).Replace('\', '/')
    $hash = (Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash.ToLowerInvariant()
    "$hash  $relative"
}
[System.IO.File]::WriteAllLines((Join-Path $bundleRoot 'SHA256SUMS.txt'), [string[]]$sums, $utf8)
Compress-Archive -Path (Join-Path $bundleRoot '*') -DestinationPath $zipPath -CompressionLevel Optimal
$null = Assert-ChisBurnerZipOsIntegrity -ZipPath $zipPath
Write-Output 'Packaged OS internal CRC: PASS (EBB2C0DC)'
Write-Output "CRC-only control package: $zipPath"
Write-Output "ZIP SHA256: $((Get-FileHash -LiteralPath $zipPath -Algorithm SHA256).Hash.ToLowerInvariant())"
