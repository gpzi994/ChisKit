[CmdletBinding()]
param(
    [string]$Distro = 'Ubuntu-24.04',
    [string]$OpenFpgaSdk = '/home/lin13/work/ONS_FPGA/openfpgaSDK',
    [string]$Version = '0.1.0-smoke.32-datatable-flush'
)

$ErrorActionPreference = 'Stop'

function Convert-ToWslPath([string]$Path) {
    $full = [System.IO.Path]::GetFullPath($Path)
    if ($full -notmatch '^([A-Za-z]):\\(.*)$') {
        throw "Expected a local Windows drive path: $full"
    }
    $drive = $Matches[1].ToLowerInvariant()
    $tail = $Matches[2].Replace('\', '/')
    return "/mnt/$drive/$tail"
}

function Invoke-WslBash([string]$Command) {
    & wsl.exe -d $Distro -- bash -lc $Command
    if ($LASTEXITCODE -ne 0) {
        throw "WSL command failed with exit code $LASTEXITCODE"
    }
}

function Require-Pattern([string]$Path, [string]$Pattern,
                         [string]$Message) {
    $text = Get-Content -Raw -LiteralPath $Path
    if ($text -notmatch $Pattern) {
        throw "$Message ($Path)"
    }
}

$projectRoot = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$integrationRoot = Join-Path $projectRoot '.integration\openfpgaos'
$platformRoot = Join-Path $projectRoot 'platform\openfpga'
$appRoot = Join-Path $projectRoot 'app\storage_stream_probe'
$artifactRoot = Join-Path $projectRoot 'build\storage-stream-artifacts'
$buildRoot = Join-Path $projectRoot 'build\smoke'
$packageName = "ChisBurner-storage-stream-$Version"
$bundleRoot = Join-Path $buildRoot $packageName
$sdRoot = Join-Path $bundleRoot 'SD_ROOT'

$coreId = 'ChisBurner.ChisBurner'
$coreRoot = Join-Path (Join-Path $sdRoot 'Cores') $coreId
$commonAssets = Join-Path $sdRoot 'Assets\chisburner\common'
$instanceAssets = Join-Path (Join-Path $sdRoot 'Assets\chisburner') $coreId
$saveAssets = Join-Path $sdRoot 'Saves\chisburner\common'
$platforms = Join-Path $sdRoot 'Platforms'

$sdkTail = $OpenFpgaSdk.TrimStart('/').Replace('/', '\')
$sdkWindows = "\\wsl.localhost\$Distro\$sdkTail"
$sdkTemplateRoot = Join-Path $sdkWindows 'src\sdk\platforms\pocket\templates'
$loader = Join-Path $sdkWindows 'runtime\pocket\loader.bin'

$bundleRootFull = [System.IO.Path]::GetFullPath($bundleRoot)
$buildRootFull = [System.IO.Path]::GetFullPath($buildRoot)
if (-not $bundleRootFull.StartsWith(
        $buildRootFull + [System.IO.Path]::DirectorySeparatorChar,
        [System.StringComparison]::OrdinalIgnoreCase)) {
    throw "Refusing to clean a bundle outside the smoke build root: $bundleRootFull"
}
if (Test-Path -LiteralPath $bundleRootFull) {
    Remove-Item -LiteralPath $bundleRootFull -Recurse -Force
}

$fpgaTargetRoot = Join-Path $integrationRoot 'src\fpga\targets\pocket'
$fpgaBuildRoot = Join-Path $fpgaTargetRoot `
    'bld\chisburner-storage-stream-s2-win'
$rbf = Join-Path $fpgaBuildRoot 'output_files\ap_core.rbf'
$qsf = Join-Path $fpgaBuildRoot 'ap_core.qsf'
$mapReport = Join-Path $fpgaBuildRoot 'output_files\ap_core.map.rpt'
$fitReport = Join-Path $fpgaBuildRoot 'output_files\ap_core.fit.rpt'
$mifUpdateReport = Join-Path $fpgaBuildRoot `
    'output_files\ap_core.mif_update.rpt'
$assemblerReport = Join-Path $fpgaBuildRoot 'output_files\ap_core.asm.rpt'
$timingReport = Join-Path $fpgaBuildRoot 'output_files\ap_core.sta.rpt'

$osBin = Join-Path $artifactRoot 'os.bin'
$probeFirmwareMif = Join-Path $artifactRoot 'firmware.mif'
$targetFirmwareMif = Join-Path $fpgaTargetRoot 'firmware.mif'
$buildFirmwareMif = Join-Path $fpgaBuildRoot 'firmware.mif'
$sourceBuildIdMif = Join-Path $fpgaTargetRoot 'apf\build_id.mif'
$buildBuildIdMif = Join-Path $fpgaBuildRoot 'apf\build_id.mif'

foreach ($required in @(
        $rbf, $qsf, $mapReport, $fitReport, $mifUpdateReport,
        $assemblerReport, $timingReport, $osBin, $probeFirmwareMif,
        $targetFirmwareMif, $buildFirmwareMif, $sourceBuildIdMif,
        $buildBuildIdMif, $loader, $sdkTemplateRoot,
        (Join-Path $platformRoot 'core.json'),
        (Join-Path $platformRoot 'storage-stream-instance.json'),
        (Join-Path $platformRoot 'info-storage-stream.txt'),
        (Join-Path $platformRoot 'interact.json'),
        (Join-Path $platformRoot 'chisburner.json'))) {
    if (-not (Test-Path -LiteralPath $required)) {
        throw "Required verified artifact is missing: $required"
    }
}

foreach ($pair in @(
        @($probeFirmwareMif, $targetFirmwareMif),
        @($probeFirmwareMif, $buildFirmwareMif),
        @($sourceBuildIdMif, $buildBuildIdMif))) {
    $leftHash = (Get-FileHash -Algorithm SHA256 `
        -LiteralPath $pair[0]).Hash
    $rightHash = (Get-FileHash -Algorithm SHA256 `
        -LiteralPath $pair[1]).Hash
    if ($leftHash -ne $rightHash) {
        throw "Stale FPGA memory image: $($pair[0]) does not match $($pair[1])"
    }
}

Require-Pattern $qsf `
    '(?im)^set_global_assignment -name VERILOG_MACRO CHISBURNER_STORAGE_STREAM\s*$' `
    'QSF is missing the isolated storage-stream build marker'
if ((Get-Content -Raw -LiteralPath $qsf) -match
        '(?im)^set_global_assignment -name VERILOG_MACRO INCLUDE_CARTRIDGE\s*$') {
    throw 'Storage diagnostic QSF must not include cartridge access'
}
Require-Pattern $mapReport `
    '(?im)^Info: Quartus Prime Analysis & Synthesis was successful\. 0 errors,' `
    'Analysis and synthesis did not complete cleanly'
Require-Pattern $fitReport `
    '(?im)^Info: Quartus Prime Fitter was successful\. 0 errors,' `
    'Fitter did not complete cleanly'
Require-Pattern $mifUpdateReport `
    '(?im)^Info: Quartus Prime MIF/HEX Update was successful\. 0 errors, 0 warnings\s*$' `
    'MIF update did not complete cleanly'
Require-Pattern $assemblerReport `
    '(?im)^Info: Quartus Prime Assembler was successful\. 0 errors, 0 warnings\s*$' `
    'Assembler did not complete cleanly'

$mifText = Get-Content -Raw -LiteralPath $mifUpdateReport
foreach ($pattern in @(
        '(?im)^\s*Info \(39025\): Processed Memory Initialization File .*[/\\]apf[/\\]build_id\.mif\b',
        '(?im)^\s*Info \(39025\): Processed Memory Initialization File .*[/\\]firmware\.mif\b')) {
    if ($mifText -notmatch $pattern) {
        throw "MIF-update report does not prove the required image was embedded: $pattern"
    }
}
if ($mifText -match '(?i)Critical Warning|can''t find|setting all initial values to 0') {
    throw 'MIF-update report contains an unsafe ROM initialization diagnostic'
}

$timingText = Get-Content -Raw -LiteralPath $timingReport
$timingMinimum = @{}
foreach ($kind in @('setup', 'hold')) {
    $matches = [regex]::Matches(
        $timingText, "Worst-case $kind slack is\s+(-?\d+(?:\.\d+)?)")
    if ($matches.Count -eq 0) {
        throw "The constrained $kind slack is missing"
    }
    $values = @($matches | ForEach-Object {
        [double]$_.Groups[1].Value
    })
    $minimum = ($values | Measure-Object -Minimum).Minimum
    if ($minimum -le 0) {
        throw "The constrained $kind slack is non-positive: $minimum"
    }
    $timingMinimum[$kind] = $minimum
}

$rbfTime = (Get-Item -LiteralPath $rbf).LastWriteTimeUtc
foreach ($inputMif in @($buildFirmwareMif, $buildBuildIdMif)) {
    if ($rbfTime -lt (Get-Item -LiteralPath $inputMif).LastWriteTimeUtc) {
        throw "RBF is older than its memory image: $inputMif"
    }
}

$appSource = Get-Content -Raw -LiteralPath (Join-Path $appRoot 'main.c')
if ($appSource -match 'of_cart_') {
    throw 'The TF-card diagnostic must not call a cartridge service'
}
if ($appSource -notmatch [regex]::Escape(
        '/Saves/chisburner/common/storage-smoke.32.bin')) {
    throw 'The expected storage-smoke output path is missing from the app'
}

$appWsl = Convert-ToWslPath $appRoot
$integrationWsl = Convert-ToWslPath $integrationRoot
Invoke-WslBash "cd '$appWsl' && make OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0"
$appElf = Join-Path $projectRoot 'build\storage-stream-app\app.elf'
if (-not (Test-Path -LiteralPath $appElf)) {
    throw "Storage-stream application build did not produce $appElf"
}

New-Item -ItemType Directory -Force $coreRoot, $commonAssets,
    $instanceAssets, $saveAssets, $platforms | Out-Null

# 0192 creates or truncates a file, but it does not create missing parent
# directories.  Ship a one-byte placeholder so extracting SD_ROOT creates the
# supported Saves tree and leaves an existing file for OPEN to resize.
$outputSeed = Join-Path $saveAssets 'storage-smoke.32.bin'
[System.IO.File]::WriteAllBytes($outputSeed, [byte[]](0))

Copy-Item -LiteralPath (Join-Path $platformRoot 'core.json') `
    -Destination $coreRoot
$coreJsonPath = Join-Path $coreRoot 'core.json'
$coreJson = Get-Content -Raw -LiteralPath $coreJsonPath | ConvertFrom-Json
$coreJson.core.metadata.version = $Version
$coreJson.core.metadata.date_release = '2026-09-03'
$coreJson.core.framework.hardware.cartridge_adapter = -1
[System.IO.File]::WriteAllText(
    $coreJsonPath, ($coreJson | ConvertTo-Json -Depth 20),
    (New-Object System.Text.UTF8Encoding($false)))

foreach ($name in @('data.json', 'video.json', 'audio.json', 'input.json',
                     'variants.json')) {
    Copy-Item -LiteralPath (Join-Path $sdkTemplateRoot $name) `
        -Destination $coreRoot
}
$dataJsonPath = Join-Path $coreRoot 'data.json'
$dataJson = Get-Content -Raw -LiteralPath $dataJsonPath | ConvertFrom-Json
$fixedSlotFiles = @{ 1 = 'os.bin'; 2 = 'chisburnersmoke.ini'; 3 = 'chisburnersmoke.elf' }
foreach ($slotId in $fixedSlotFiles.Keys) {
    $slot = $dataJson.data.data_slots | Where-Object { $_.id -eq $slotId }
    if (-not $slot) {
        throw "SDK data.json is missing required slot $slotId"
    }
    $slot | Add-Member -NotePropertyName filename `
        -NotePropertyValue $fixedSlotFiles[$slotId] -Force
}
$slot4 = $dataJson.data.data_slots | Where-Object { $_.id -eq 4 }
if (-not $slot4 -or $slot4.deferload -ne $true -or
        [int]$slot4.parameters -ne 0 -or
        $slot4.PSObject.Properties.Name -contains 'filename' -or
        $slot4.PSObject.Properties.Name -contains 'nonvolatile') {
    throw 'SDK slot 4 is not an unbound writable deferred data slot'
}
[System.IO.File]::WriteAllText(
    $dataJsonPath, ($dataJson | ConvertTo-Json -Depth 20),
    (New-Object System.Text.UTF8Encoding($false)))

Copy-Item -LiteralPath (Join-Path $platformRoot 'interact.json') `
    -Destination $coreRoot
Copy-Item -LiteralPath (Join-Path $platformRoot 'info-storage-stream.txt') `
    -Destination (Join-Path $coreRoot 'info.txt')
Copy-Item -LiteralPath $loader -Destination (Join-Path $coreRoot 'loader.bin')
Copy-Item -LiteralPath $osBin -Destination (Join-Path $commonAssets 'os.bin')
Copy-Item -LiteralPath $appElf `
    -Destination (Join-Path $commonAssets 'chisburnersmoke.elf')
Copy-Item -LiteralPath (Join-Path $appRoot 'chisburnersmoke.ini') `
    -Destination $commonAssets
Copy-Item -LiteralPath (Join-Path $platformRoot `
    'storage-stream-instance.json') `
    -Destination (Join-Path $instanceAssets 'TF Storage Stream.json')
Copy-Item -LiteralPath (Join-Path $platformRoot 'chisburner.json') `
    -Destination $platforms

$toolDir = Join-Path $buildRoot 'tools'
New-Item -ItemType Directory -Force $toolDir | Out-Null
$toolWsl = Convert-ToWslPath (Join-Path $toolDir 'reverse_bits')
$rbfWsl = Convert-ToWslPath $rbf
$rbfR = Join-Path $coreRoot 'chis.rbf_r'
$rbfRWsl = Convert-ToWslPath $rbfR
$roundTrip = Join-Path $toolDir 'roundtrip.rbf'
$roundTripWsl = Convert-ToWslPath $roundTrip
Invoke-WslBash "gcc -O2 -o '$toolWsl' '$integrationWsl/tools/reverse_bits.c'"
Invoke-WslBash "'$toolWsl' '$rbfWsl' '$rbfRWsl'"
Invoke-WslBash "'$toolWsl' '$rbfRWsl' '$roundTripWsl'"
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $rbf).Hash -ne
        (Get-FileHash -Algorithm SHA256 -LiteralPath $roundTrip).Hash) {
    throw 'RBF bit-reversal round-trip verification failed'
}
Remove-Item -LiteralPath $roundTrip -Force

$core = Get-Content -Raw -LiteralPath $coreJsonPath | ConvertFrom-Json
if ([int64]$core.core.framework.hardware.cartridge_adapter -ne -1) {
    throw 'Storage diagnostic metadata must keep cartridge power off'
}
if ($core.core.metadata.version -ne $Version) {
    throw "core.json version does not match package version $Version"
}
$metadataCoreId = "$($core.core.metadata.author).$($core.core.metadata.shortname)"
if ($metadataCoreId -cne $coreId) {
    throw "Core directory identity mismatch: $metadataCoreId"
}
foreach ($field in @{ shortname=31; description=63; author=31; url=63;
                      version=31; date_release=10 }.Keys) {
    $limit = @{ shortname=31; description=63; author=31; url=63;
                version=31; date_release=10 }[$field]
    if (([string]$core.core.metadata.$field).Length -gt $limit) {
        throw "core.json metadata.$field exceeds $limit characters"
    }
}
if ($core.core.cores[0].filename.Length -gt 15 -or
        $core.core.cores[0].chip32_vm.Length -gt 15) {
    throw 'Pocket core filename exceeds the 15-character loader limit'
}
Get-ChildItem -LiteralPath $sdRoot -Filter '*.json' -File -Recurse |
    ForEach-Object {
        $null = Get-Content -Raw -LiteralPath $_.FullName | ConvertFrom-Json
    }

$readme = @"
ChisBurner - TF storage streaming smoke $Version

Run this test with the cartridge slot EMPTY.

Merge the complete SD_ROOT into the SD-card root and approve replacement of all
files.  This smoke cannot be installed by replacing only the usual three files,
because its fourth required payload creates the missing Saves directory:
  Cores/ChisBurner.ChisBurner/chis.rbf_r
  Assets/chisburner/common/os.bin
  Assets/chisburner/common/chisburnersmoke.elf
  Saves/chisburner/common/storage-smoke.32.bin

Start openFPGA -> ChisBurner -> TF Storage Stream normally. Do not use
Play Cartridge for this package.

The application screen must say "TF STORAGE smoke.32". If it shows SAFE-IDLE,
GBA ADDRESS, or an older smoke number, replacement did not take effect; delete
the stale ChisBurner core/asset directories and merge SD_ROOT again.

Expected final screen:
  OUTPUT API: PASS
  OPEN:  PASS RC=0
  OPEN HOST: OK
  WRITE: PASS RC=0
  BYTES: 1048613 / 1048613
  CHUNKS: 33 (LAST 37)
  CRC32: e3e64e77 PASS
  FLUSH: PASS RC=0
  STORAGE STREAM: PASS
  DIAG COMPLETE

If FLUSH still fails, copy the bracketed timeout line immediately above
"FLUSH DIAG ABOVE" as well as the final FLUSH line.  The timeout line names
the exact phase (PREWAIT, ACK, DONE, READY, or WR_IDLE) and includes raw status.

Then power Pocket off, read the card on a computer, and verify:
  file: Saves/chisburner/common/storage-smoke.32.bin
  size: 1048613 bytes
  MD5:  b58dc8fe81364ad8cf5e30dbf7388ec3
  SHA256: 6cefa9b055d371f28b0fd2aa3f5d1acda6426a99a59e8e9fad0ee7c0030d6900

Every run intentionally recreates and truncates the same diagnostic file.
This package includes a one-byte placeholder at that path so the Saves parent
directory exists before the application issues OPEN.
This package does not request cartridge access and does not read or write a cartridge.

Quartus: full compile passed. Minimum setup slack: $($timingMinimum.setup) ns.
Minimum hold slack: $($timingMinimum.hold) ns.
"@
[System.IO.File]::WriteAllText(
    (Join-Path $bundleRoot 'README_TEST.txt'), $readme,
    (New-Object System.Text.UTF8Encoding($false)))

$provenance = @"
version=$Version
variant=chisburner_storage_stream
quartus=25.1std.0
quartus_full_compile=PASS
quartus_mif_update=PASS
minimum_setup_slack_ns=$($timingMinimum.setup)
minimum_hold_slack_ns=$($timingMinimum.hold)
rbf_sha256=$((Get-FileHash -Algorithm SHA256 -LiteralPath $rbf).Hash.ToLowerInvariant())
os_sha256=$((Get-FileHash -Algorithm SHA256 -LiteralPath $osBin).Hash.ToLowerInvariant())
app_sha256=$((Get-FileHash -Algorithm SHA256 -LiteralPath $appElf).Hash.ToLowerInvariant())
firmware_mif_sha256=$((Get-FileHash -Algorithm SHA256 -LiteralPath $probeFirmwareMif).Hash.ToLowerInvariant())
payload_size=1048613
payload_crc32=e3e64e77
payload_md5=b58dc8fe81364ad8cf5e30dbf7388ec3
payload_sha256=6cefa9b055d371f28b0fd2aa3f5d1acda6426a99a59e8e9fad0ee7c0030d6900
"@
[System.IO.File]::WriteAllText(
    (Join-Path $bundleRoot 'BUILD_PROVENANCE.txt'), $provenance,
    (New-Object System.Text.UTF8Encoding($false)))

$sdRootPrefix = [System.IO.Path]::GetFullPath($sdRoot).TrimEnd('\') + '\'
$checksumLines = Get-ChildItem -LiteralPath $sdRoot -File -Recurse |
    Sort-Object FullName |
    ForEach-Object {
        $relative = $_.FullName.Substring($sdRootPrefix.Length).Replace('\', '/')
        $hash = (Get-FileHash -Algorithm SHA256 `
            -LiteralPath $_.FullName).Hash.ToLowerInvariant()
        "$hash  $relative"
    }
[System.IO.File]::WriteAllLines(
    (Join-Path $bundleRoot 'SHA256SUMS.txt'), [string[]]$checksumLines,
    (New-Object System.Text.UTF8Encoding($false)))

$zipPath = "$bundleRoot.zip"
if (Test-Path -LiteralPath $zipPath) {
    Remove-Item -LiteralPath $zipPath -Force
}
Compress-Archive -Path (Join-Path $bundleRoot '*') `
    -DestinationPath $zipPath -CompressionLevel Optimal

Write-Host "TF storage-stream smoke package: $zipPath"
