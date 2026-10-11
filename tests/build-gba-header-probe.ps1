[CmdletBinding()]
param(
    [string]$Distro = 'Ubuntu-24.04',
    [string]$OpenFpgaSdk = '/home/lin13/work/ONS_FPGA/openfpgaSDK',
    [ValidateSet('Header', 'Rom4K', 'Address')]
    [string]$Probe = 'Header',
    [string]$Version = ''
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

function Require-Pattern([string]$Path, [string]$Pattern, [string]$Message) {
    $text = Get-Content -Raw -LiteralPath $Path
    if ($text -notmatch $Pattern) {
        throw "$Message ($Path)"
    }
}

$projectRoot = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$integrationRoot = Join-Path $projectRoot '.integration\openfpgaos'
$platformRoot = Join-Path $projectRoot 'platform\openfpga'
$buildRoot = Join-Path $projectRoot 'build\smoke'
if ($Probe -eq 'Address') {
    if ([string]::IsNullOrWhiteSpace($Version)) {
        $Version = '0.1.0-smoke.27-gba-address'
    }
    $appDirectory = 'gba_addr_probe'
    $appBuildDirectory = 'gba-addr-probe-app'
    $artifactDirectory = 'gba-addr-probe-artifacts'
    $fpgaBuildDirectory = 'chisburner-gba-addr-probe-s2-win'
    $packagePrefix = 'ChisBurner-gba-address'
    $qsfMacro = 'CHISBURNER_GBA_ADDR_PROBE'
    $infoFile = 'info-gba-addr.txt'
    $instanceFile = 'gba-addr-probe-instance.json'
    $instanceName = 'GBA Address Probe.json'
    $variantName = 'chisburner_gba_addr_probe'
} elseif ($Probe -eq 'Rom4K') {
    if ([string]::IsNullOrWhiteSpace($Version)) {
        $Version = '0.1.0-smoke.26-gba-rom4k'
    }
    $appDirectory = 'gba_rom4k_probe'
    $appBuildDirectory = 'gba-rom4k-probe-app'
    $artifactDirectory = 'gba-rom4k-probe-artifacts'
    $fpgaBuildDirectory = 'chisburner-gba-rom4k-probe-win'
    $packagePrefix = 'ChisBurner-gba-rom4k'
    $qsfMacro = 'CHISBURNER_GBA_ROM4K_PROBE'
    $infoFile = 'info-gba-rom4k.txt'
    $instanceFile = 'gba-rom4k-probe-instance.json'
    $instanceName = 'GBA ROM 4K Probe.json'
    $variantName = 'chisburner_gba_rom4k_probe'
} else {
    if ([string]::IsNullOrWhiteSpace($Version)) {
        $Version = '0.1.0-smoke.25-gba-header'
    }
    $appDirectory = 'gba_header_probe'
    $appBuildDirectory = 'gba-header-probe-app'
    $artifactDirectory = 'gba-header-probe-artifacts'
    $fpgaBuildDirectory = 'chisburner-gba-probe-win'
    $packagePrefix = 'ChisBurner-gba-header'
    $qsfMacro = 'CHISBURNER_GBA_HEADER_PROBE'
    $infoFile = 'info-gba-header.txt'
    $instanceFile = 'gba-header-probe-instance.json'
    $instanceName = 'GBA Header Probe.json'
    $variantName = 'chisburner_gba_probe'
}
$appRoot = Join-Path (Join-Path $projectRoot 'app') $appDirectory
$artifactRoot = Join-Path (Join-Path $projectRoot 'build') $artifactDirectory
$packageName = "$packagePrefix-$Version"
$bundleRoot = Join-Path $buildRoot $packageName
$sdRoot = Join-Path $bundleRoot 'SD_ROOT'
$coreId = 'ChisBurner.ChisBurner'
$coreRoot = Join-Path (Join-Path $sdRoot 'Cores') $coreId
$commonAssets = Join-Path $sdRoot 'Assets\chisburner\common'
$instanceAssets = Join-Path (Join-Path $sdRoot 'Assets\chisburner') $coreId
$platforms = Join-Path $sdRoot 'Platforms'

$sdkTail = $OpenFpgaSdk.TrimStart('/').Replace('/', '\')
$sdkWindows = "\\wsl.localhost\$Distro\$sdkTail"
$sdkTemplateRoot = Join-Path $sdkWindows 'src\sdk\platforms\pocket\templates'
$loader = Join-Path $sdkWindows 'runtime\pocket\loader.bin'

$buildRootFull = [System.IO.Path]::GetFullPath($buildRoot)
$bundleRootFull = [System.IO.Path]::GetFullPath($bundleRoot)
if (-not $bundleRootFull.StartsWith($buildRootFull +
        [System.IO.Path]::DirectorySeparatorChar,
        [System.StringComparison]::OrdinalIgnoreCase)) {
    throw "Refusing to clean a bundle outside the smoke build root: $bundleRootFull"
}
if (Test-Path -LiteralPath $bundleRootFull) {
    Remove-Item -LiteralPath $bundleRootFull -Recurse -Force
}

$fpgaTargetRoot = Join-Path $integrationRoot 'src\fpga\targets\pocket'
$fpgaBuildRoot = Join-Path (Join-Path $fpgaTargetRoot 'bld') $fpgaBuildDirectory
$rbf = Join-Path $fpgaBuildRoot 'output_files\ap_core.rbf'
$osBin = Join-Path $artifactRoot 'os.bin'
$probeFirmwareMif = Join-Path $artifactRoot 'firmware.mif'
$targetFirmwareMif = Join-Path $fpgaTargetRoot 'firmware.mif'
$buildFirmwareMif = Join-Path $fpgaBuildRoot 'firmware.mif'
$sourceBuildIdMif = Join-Path $fpgaTargetRoot 'apf\build_id.mif'
$buildBuildIdMif = Join-Path $fpgaBuildRoot 'apf\build_id.mif'
$qsf = Join-Path $fpgaBuildRoot 'ap_core.qsf'
$mapReport = Join-Path $fpgaBuildRoot 'output_files\ap_core.map.rpt'
$fitReport = Join-Path $fpgaBuildRoot 'output_files\ap_core.fit.rpt'
$mifUpdateReport = Join-Path $fpgaBuildRoot 'output_files\ap_core.mif_update.rpt'
$assemblerReport = Join-Path $fpgaBuildRoot 'output_files\ap_core.asm.rpt'
$timingReport = Join-Path $fpgaBuildRoot 'output_files\ap_core.sta.rpt'

foreach ($required in @($rbf, $osBin, $probeFirmwareMif,
                         $targetFirmwareMif, $buildFirmwareMif,
                         $sourceBuildIdMif, $buildBuildIdMif, $qsf,
                         $mapReport, $fitReport, $mifUpdateReport,
                         $assemblerReport, $timingReport, $loader,
                         $sdkTemplateRoot,
                         (Join-Path $platformRoot $infoFile),
                         (Join-Path $platformRoot $instanceFile))) {
    if (-not (Test-Path -LiteralPath $required)) {
        throw "Required verified artifact is missing: $required"
    }
}

foreach ($pair in @(@($probeFirmwareMif, $targetFirmwareMif),
                     @($probeFirmwareMif, $buildFirmwareMif),
                     @($sourceBuildIdMif, $buildBuildIdMif))) {
    $leftHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $pair[0]).Hash
    $rightHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $pair[1]).Hash
    if ($leftHash -ne $rightHash) {
        throw "Stale FPGA memory image: $($pair[0]) does not match $($pair[1])"
    }
}

Require-Pattern $qsf ("(?im)^set_global_assignment -name VERILOG_MACRO " +
    [regex]::Escape($qsfMacro) + "\s*$") `
    "Probe QSF is missing the isolated $qsfMacro macro"
Require-Pattern $mapReport '(?im)^Info: Quartus Prime Analysis & Synthesis was successful\. 0 errors,' `
    'Analysis and synthesis did not complete cleanly'
Require-Pattern $fitReport '(?im)^Info: Quartus Prime Fitter was successful\. 0 errors,' `
    'Fitter did not complete cleanly'
Require-Pattern $mifUpdateReport '(?im)^Info: Quartus Prime MIF/HEX Update was successful\. 0 errors, 0 warnings\s*$' `
    'MIF update did not complete cleanly'
Require-Pattern $assemblerReport '(?im)^Info: Quartus Prime Assembler was successful\. 0 errors, 0 warnings\s*$' `
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
    $matches = [regex]::Matches($timingText,
        "Worst-case $kind slack is\s+(-?\d+(?:\.\d+)?)")
    if ($matches.Count -eq 0) {
        throw "The latest constrained $kind slack is missing"
    }
    $values = @($matches | ForEach-Object { [double]$_.Groups[1].Value })
    $minimum = ($values | Measure-Object -Minimum).Minimum
    if ($minimum -le 0) {
        throw "The latest constrained $kind slack is non-positive: $minimum"
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
if ($appSource -match 'of_cart_(write|erase|program)') {
    throw 'The header probe application contains a cartridge mutation call'
}

$appWsl = Convert-ToWslPath $appRoot
$integrationWsl = Convert-ToWslPath $integrationRoot
Invoke-WslBash "cd '$appWsl' && make OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0"
$appElf = Join-Path (Join-Path (Join-Path $projectRoot 'build') $appBuildDirectory) 'app.elf'
if (-not (Test-Path -LiteralPath $appElf)) {
    throw "Header probe application build did not produce $appElf"
}

New-Item -ItemType Directory -Force $coreRoot, $commonAssets,
    $instanceAssets, $platforms | Out-Null

Copy-Item -LiteralPath (Join-Path $platformRoot 'core.json') -Destination $coreRoot
$coreJsonPath = Join-Path $coreRoot 'core.json'
$coreJson = Get-Content -Raw -LiteralPath $coreJsonPath | ConvertFrom-Json
$coreJson.core.metadata.version = $Version
$coreJson.core.metadata.date_release = '2026-09-02'
[System.IO.File]::WriteAllText($coreJsonPath,
    ($coreJson | ConvertTo-Json -Depth 20),
    (New-Object System.Text.UTF8Encoding($false)))

foreach ($name in @('data.json', 'video.json', 'audio.json', 'input.json',
                     'variants.json')) {
    Copy-Item -LiteralPath (Join-Path $sdkTemplateRoot $name) -Destination $coreRoot
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
[System.IO.File]::WriteAllText($dataJsonPath,
    ($dataJson | ConvertTo-Json -Depth 20),
    (New-Object System.Text.UTF8Encoding($false)))

Copy-Item -LiteralPath (Join-Path $platformRoot 'interact.json') -Destination $coreRoot
Copy-Item -LiteralPath (Join-Path $platformRoot $infoFile) `
    -Destination (Join-Path $coreRoot 'info.txt')
Copy-Item -LiteralPath $loader -Destination (Join-Path $coreRoot 'loader.bin')
Copy-Item -LiteralPath $osBin -Destination (Join-Path $commonAssets 'os.bin')
Copy-Item -LiteralPath $appElf -Destination (Join-Path $commonAssets 'chisburnersmoke.elf')
Copy-Item -LiteralPath (Join-Path $appRoot 'chisburnersmoke.ini') -Destination $commonAssets
Copy-Item -LiteralPath (Join-Path $platformRoot $instanceFile) `
    -Destination (Join-Path $instanceAssets $instanceName)
Copy-Item -LiteralPath (Join-Path $platformRoot 'chisburner.json') -Destination $platforms

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
if ($core.core.framework.hardware.cartridge_adapter -ne 16777216) {
    throw 'Probe metadata must request Play Cartridge (bit 24 only)'
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
    ForEach-Object { $null = Get-Content -Raw -LiteralPath $_.FullName | ConvertFrom-Json }

if ($Probe -eq 'Address') {
    $readme = @"
ChisBurner - GBA high-address probe $Version

Use a known genuine GBA cartridge. Power Pocket off before inserting it.

Overwrite these three files on the SD card (do not merely copy beside stale files):
  Cores/ChisBurner.ChisBurner/chis.rbf_r
  Assets/chisburner/common/os.bin
  Assets/chisburner/common/chisburnersmoke.elf

Alternatively merge SD_ROOT into the SD-card root and approve replacement of all files.
Then open openFPGA -> ChisBurner -> Play Cartridge.

The application screen must say "GBA ADDRESS smoke.27". If it shows GBA ROM 4K,
GBA HEADER, SAFE-IDLE, or an older smoke number, replacement did not take effect;
delete the stale ChisBurner core/asset directories and merge SD_ROOT again.

Expected result with a valid GBA cartridge:
  PASS1: 11/11 RC=0
  PASS2: 11/11 RC=0
  CRC1 and CRC2 are identical
  MIRROR SIZE: a power-of-two size (information only)
  CRC MATCH: PASS
  DATA MATCH: PASS
  HIGH 1FFFFE0: PASS
  GBA HEADER: PASS
  FINAL IDLE: PASS
  GBA ADDRESS PROBE: PASS

This diagnostic performs two passes over eleven fixed aligned 32-byte ROM windows:
the ChisBurner-style power-of-two mirror probe addresses 0x000000A0-0x010000A0 and
the final aligned block at 0x01FFFFE0. Both OS and FPGA reject every other address
or length, and the FPGA rejects a 23rd command. MIRROR SIZE is informational because
smaller cartridges normally mirror below 32 MiB. WR# is constant inactive; no
cartridge write-data path or write command exists. Save memory is not accessed.
Exit and reload the core before repeating this one-shot test.

Quartus: full compile passed. Minimum setup slack: $($timingMinimum.setup) ns.
Minimum hold slack: $($timingMinimum.hold) ns.
"@
} elseif ($Probe -eq 'Rom4K') {
    $readme = @"
ChisBurner - GBA ROM 4 KiB probe $Version

Use a known genuine GBA cartridge. Power Pocket off before inserting it.

Overwrite these three files on the SD card (do not merely copy beside stale files):
  Cores/ChisBurner.ChisBurner/chis.rbf_r
  Assets/chisburner/common/os.bin
  Assets/chisburner/common/chisburnersmoke.elf

Alternatively merge SD_ROOT into the SD-card root and approve replacement of all files.
Then open openFPGA -> ChisBurner -> Play Cartridge.

The application screen must say "GBA ROM 4K smoke.26". If it shows GBA HEADER,
SAFE-IDLE, PROTOCOL, or an older smoke number, replacement did not take effect; delete
the stale ChisBurner core/asset directories and merge SD_ROOT again.

Expected result with a valid GBA cartridge:
  PASS1: 128/128 RC=0
  PASS2: 128/128 RC=0
  CRC1 and CRC2 are identical
  CRC MATCH: PASS
  DATA MATCH: PASS
  GBA HEADER: PASS
  FINAL IDLE: PASS
  GBA ROM 4K PROBE: PASS

This diagnostic performs two passes of aligned 32-byte ROM reads over byte addresses
0x00000000-0x00000FFF. Both the OS and FPGA reject every other address or length, and
the FPGA rejects a 257th command. The ROM timing is intentionally conservative. WR#
is constant inactive; no cartridge write-data path or write command exists. Save
memory is not accessed. Exit and reload the core before repeating this one-shot test.

Quartus: full compile passed. Minimum setup slack: $($timingMinimum.setup) ns.
Minimum hold slack: $($timingMinimum.hold) ns.
"@
} else {
    $readme = @"
ChisBurner - GBA ROM header probe $Version

FIRST TEST: use a known genuine GBA cartridge. Power Pocket off before inserting it.

Overwrite these three files on the SD card (do not merely copy beside stale files):
  Cores/ChisBurner.ChisBurner/chis.rbf_r
  Assets/chisburner/common/os.bin
  Assets/chisburner/common/chisburnersmoke.elf

Alternatively merge SD_ROOT into the SD-card root and approve replacement of all files.
Then open openFPGA -> ChisBurner -> Play Cartridge.

The application screen must say "GBA HEADER smoke.25". If it still says SAFE-IDLE,
PROTOCOL, or an older smoke number, replacement did not take effect; delete the stale
ChisBurner core/asset directories and merge SD_ROOT again.

Expected result with a valid GBA cartridge:
  READ1/READ2: 0 / 0
  REPEAT: PASS
  FIXED96: PASS
  CHECKSUM: PASS
  GBA HEADER PROBE: PASS

This diagnostic performs exactly two 32-byte ROM reads at byte address 0xA0. Both the
OS and FPGA reject every other address/length, and the FPGA rejects a third command.
The ROM timing is intentionally conservative. WR# is constant inactive; no cartridge
write-data path or write command exists. Save memory is not accessed.

Quartus: full compile passed. Minimum setup slack: $($timingMinimum.setup) ns.
Minimum hold slack: $($timingMinimum.hold) ns.
"@
}
[System.IO.File]::WriteAllText((Join-Path $bundleRoot 'README_TEST.txt'), $readme,
    (New-Object System.Text.UTF8Encoding($false)))

$provenance = @"
version=$Version
variant=$variantName
quartus=25.1std.0
quartus_full_compile=PASS
minimum_setup_slack_ns=$($timingMinimum.setup)
minimum_hold_slack_ns=$($timingMinimum.hold)
rbf_sha256=$((Get-FileHash -Algorithm SHA256 -LiteralPath $rbf).Hash.ToLowerInvariant())
os_sha256=$((Get-FileHash -Algorithm SHA256 -LiteralPath $osBin).Hash.ToLowerInvariant())
app_sha256=$((Get-FileHash -Algorithm SHA256 -LiteralPath $appElf).Hash.ToLowerInvariant())
firmware_mif_sha256=$((Get-FileHash -Algorithm SHA256 -LiteralPath $probeFirmwareMif).Hash.ToLowerInvariant())
"@
[System.IO.File]::WriteAllText((Join-Path $bundleRoot 'BUILD_PROVENANCE.txt'),
    $provenance, (New-Object System.Text.UTF8Encoding($false)))

$sdRootPrefix = [System.IO.Path]::GetFullPath($sdRoot).TrimEnd('\') + '\'
$checksumLines = Get-ChildItem -LiteralPath $sdRoot -File -Recurse |
    Sort-Object FullName |
    ForEach-Object {
        $relative = $_.FullName.Substring($sdRootPrefix.Length).Replace('\', '/')
        $hash = (Get-FileHash -Algorithm SHA256 -LiteralPath $_.FullName).Hash.ToLowerInvariant()
        "$hash  $relative"
    }
[System.IO.File]::WriteAllLines((Join-Path $bundleRoot 'SHA256SUMS.txt'),
    [string[]]$checksumLines, (New-Object System.Text.UTF8Encoding($false)))

$zipPath = "$bundleRoot.zip"
if (Test-Path -LiteralPath $zipPath) {
    Remove-Item -LiteralPath $zipPath -Force
}
Compress-Archive -Path (Join-Path $bundleRoot '*') -DestinationPath $zipPath `
    -CompressionLevel Optimal

Write-Host "GBA $Probe probe package: $zipPath"
