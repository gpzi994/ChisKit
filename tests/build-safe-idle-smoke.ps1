[CmdletBinding()]
param(
    [string]$Distro = 'Ubuntu-24.04',
    [string]$OpenFpgaSdk = '/home/lin13/work/ONS_FPGA/openfpgaSDK',
    [string]$Version = '0.1.0-smoke.24-query-ping'
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

$projectRoot = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$integrationRoot = Join-Path $projectRoot '.integration\openfpgaos'
$appRoot = Join-Path $projectRoot 'app\safe_idle_smoke'
$platformRoot = Join-Path $projectRoot 'platform\openfpga'
$buildRoot = Join-Path $projectRoot 'build\smoke'
$packageName = "ChisBurner-protocol-$Version"
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

$buildRootFull = [System.IO.Path]::GetFullPath($buildRoot)
$bundleRootFull = [System.IO.Path]::GetFullPath($bundleRoot)
if (-not $bundleRootFull.StartsWith($buildRootFull + [System.IO.Path]::DirectorySeparatorChar,
                                    [System.StringComparison]::OrdinalIgnoreCase)) {
    throw "Refusing to clean a bundle outside the smoke build root: $bundleRootFull"
}
if (Test-Path -LiteralPath $bundleRootFull) {
    Remove-Item -LiteralPath $bundleRootFull -Recurse -Force
}

$fpgaTargetRoot = Join-Path $integrationRoot 'src\fpga\targets\pocket'
$fpgaBuildRoot = Join-Path $fpgaTargetRoot 'bld\chisburner-win'
$rbf = Join-Path $fpgaBuildRoot 'output_files\ap_core.rbf'
$osBin = Join-Path $integrationRoot 'src\firmware\os\bld\pocket\os.bin'
$generatedFirmwareMif = Join-Path $integrationRoot 'src\firmware\os\bld\pocket\firmware.mif'
$targetFirmwareMif = Join-Path $fpgaTargetRoot 'firmware.mif'
$buildFirmwareMif = Join-Path $fpgaBuildRoot 'firmware.mif'
$sourceBuildIdMif = Join-Path $fpgaTargetRoot 'apf\build_id.mif'
$buildBuildIdMif = Join-Path $fpgaBuildRoot 'apf\build_id.mif'
$mifUpdateReport = Join-Path $fpgaBuildRoot 'output_files\ap_core.mif_update.rpt'
$assemblerReport = Join-Path $fpgaBuildRoot 'output_files\ap_core.asm.rpt'
$timingReport = Join-Path $fpgaBuildRoot 'output_files\ap_core.sta.rpt'
$loader = "\\wsl.localhost\$Distro\home\lin13\work\ONS_FPGA\openfpgaSDK\runtime\pocket\loader.bin"
foreach ($required in @($rbf, $osBin, $loader, $generatedFirmwareMif,
                         $targetFirmwareMif, $buildFirmwareMif,
                         $sourceBuildIdMif, $buildBuildIdMif,
                         $mifUpdateReport, $assemblerReport, $timingReport,
                         $sdkTemplateRoot,
                         (Join-Path $platformRoot 'info-safe-idle.txt'))) {
    if (-not (Test-Path -LiteralPath $required)) {
        throw "Required verified artifact is missing: $required"
    }
}

foreach ($pair in @(@($generatedFirmwareMif, $targetFirmwareMif),
                     @($targetFirmwareMif, $buildFirmwareMif),
                     @($sourceBuildIdMif, $buildBuildIdMif))) {
    $leftHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $pair[0]).Hash
    $rightHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $pair[1]).Hash
    if ($leftHash -ne $rightHash) {
        throw "Stale FPGA memory image: $($pair[0]) does not match $($pair[1])"
    }
}

$buildIdText = Get-Content -Raw -LiteralPath $buildBuildIdMif
if ($buildIdText -notmatch '(?im)^\s*DEPTH\s*=\s*1024\s*;') {
    throw 'build_id.mif must cover the 1024-word APF data table'
}

$mifText = Get-Content -Raw -LiteralPath $mifUpdateReport
foreach ($pattern in @(
        '(?im)^\s*Info \(39025\): Processed Memory Initialization File .*[/\\]apf[/\\]build_id\.mif\b',
        '(?im)^\s*Info \(39025\): Processed Memory Initialization File .*[/\\]firmware\.mif\b',
        '(?im)^Info: Quartus Prime MIF/HEX Update was successful\. 0 errors, 0 warnings\s*$')) {
    if ($mifText -notmatch $pattern) {
        throw "Quartus MIF-update report does not prove the required ROM image was embedded: $pattern"
    }
}
if ($mifText -match '(?i)Critical Warning|can''t find|setting all initial values to 0') {
    throw 'Quartus MIF-update report contains an unsafe ROM initialization diagnostic'
}

$rbfTime = (Get-Item -LiteralPath $rbf).LastWriteTimeUtc
foreach ($inputMif in @($buildFirmwareMif, $buildBuildIdMif)) {
    if ($rbfTime -lt (Get-Item -LiteralPath $inputMif).LastWriteTimeUtc) {
        throw "RBF is older than its memory image: $inputMif"
    }
}

$assemblerText = Get-Content -Raw -LiteralPath $assemblerReport
if ($assemblerText -notmatch '(?im)^Info: Quartus Prime Assembler was successful\. 0 errors, 0 warnings\s*$') {
    throw 'The latest Quartus assembler report is not clean'
}

$timingText = Get-Content -Raw -LiteralPath $timingReport
foreach ($kind in @('setup', 'hold')) {
    $match = [regex]::Match($timingText, "Worst-case $kind slack is\s+(-?\d+(?:\.\d+)?)")
    if (-not $match.Success -or [double]$match.Groups[1].Value -le 0) {
        throw "The latest constrained $kind slack is missing or non-positive"
    }
}

$projectWsl = Convert-ToWslPath $projectRoot
$appWsl = Convert-ToWslPath $appRoot
$integrationWsl = Convert-ToWslPath $integrationRoot

Invoke-WslBash "cd '$appWsl' && make OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0"
$appElf = Join-Path $projectRoot 'build\smoke-app\app.elf'
if (-not (Test-Path -LiteralPath $appElf)) {
    throw "Smoke application build did not produce $appElf"
}

New-Item -ItemType Directory -Force $coreRoot, $commonAssets, $instanceAssets, $platforms | Out-Null

Copy-Item -LiteralPath (Join-Path $platformRoot 'core.json') -Destination $coreRoot
foreach ($name in @('data.json', 'video.json', 'audio.json', 'input.json',
                     'variants.json')) {
    Copy-Item -LiteralPath (Join-Path $sdkTemplateRoot $name) -Destination $coreRoot
}
$dataJsonPath = Join-Path $coreRoot 'data.json'
$dataJson = Get-Content -Raw -LiteralPath $dataJsonPath | ConvertFrom-Json
$fixedSlotFiles = @{
    1 = 'os.bin'
    2 = 'chisburnersmoke.ini'
    3 = 'chisburnersmoke.elf'
}
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
Copy-Item -LiteralPath (Join-Path $platformRoot 'info-safe-idle.txt') `
    -Destination (Join-Path $coreRoot 'info.txt')
Copy-Item -LiteralPath $loader -Destination (Join-Path $coreRoot 'loader.bin')
Copy-Item -LiteralPath $osBin -Destination (Join-Path $commonAssets 'os.bin')
Copy-Item -LiteralPath $appElf -Destination (Join-Path $commonAssets 'chisburnersmoke.elf')
Copy-Item -LiteralPath (Join-Path $appRoot 'chisburnersmoke.ini') -Destination $commonAssets
Copy-Item -LiteralPath (Join-Path $platformRoot 'safe-idle-smoke-instance.json') `
    -Destination (Join-Path $instanceAssets 'Safe Idle Smoke.json')
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

$sourceHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $rbf).Hash
$roundTripHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $roundTrip).Hash
if ($sourceHash -ne $roundTripHash) {
    throw 'RBF bit-reversal round-trip verification failed'
}
Remove-Item -LiteralPath $roundTrip -Force

$core = Get-Content -Raw (Join-Path $coreRoot 'core.json') | ConvertFrom-Json
if ($core.core.framework.hardware.cartridge_adapter -ne 16777216) {
    throw 'Lifecycle smoke metadata must request Play Cartridge (bit 24 only)'
}
if ($core.core.metadata.version -ne $Version) {
    throw "core.json version does not match package version $Version"
}
$metadataCoreId = "$($core.core.metadata.author).$($core.core.metadata.shortname)"
if ($metadataCoreId -cne $coreId) {
    throw "Core directory identity mismatch: metadata requires '$metadataCoreId', package uses '$coreId'"
}
if ((Split-Path -Leaf $coreRoot) -cne $metadataCoreId -or
    (Split-Path -Leaf $instanceAssets) -cne $metadataCoreId) {
    throw 'Core and instance directories must exactly match metadata author.shortname'
}
$metadataLimits = @{
    shortname = 31
    description = 63
    author = 31
    url = 63
    version = 31
    date_release = 10
}
foreach ($field in $metadataLimits.Keys) {
    $value = [string]$core.core.metadata.$field
    if ($value.Length -gt $metadataLimits[$field]) {
        throw "core.json metadata.$field exceeds $($metadataLimits[$field]) characters"
    }
}
if ($core.core.cores[0].name.Length -gt 15) {
    throw 'Pocket bitstream debug name exceeds 15 characters'
}
if ($core.core.cores[0].filename.Length -gt 15) {
    throw 'Pocket bitstream filename exceeds 15 characters'
}
if ($core.core.cores[0].chip32_vm.Length -gt 15) {
    throw 'Pocket Chip32 filename exceeds 15 characters'
}
Get-ChildItem -LiteralPath $sdRoot -Filter '*.json' -File -Recurse | ForEach-Object {
    $null = Get-Content -Raw -LiteralPath $_.FullName | ConvertFrom-Json
}

$install = @"
ChisBurner - protocol query/ping smoke $Version

1. Remove any cartridge from Pocket.
2. Back up and remove the existing ChisBurner.ChisBurner core and
   instance directories so files from the SDK-baseline package cannot remain.
3. Merge the contents of SD_ROOT into the SD-card root. Do not remove other cores.
4. On Pocket open: openFPGA -> ChisBurner. In that core's asset browser,
   choose Play Cartridge instead of Safe Idle Smoke. Do not use the similarly
   named Play Cartridge item on Pocket's main menu.
5. Report QUERY, PING, LIFE RAW, B1 RAW, HOST DIAG, and COUNT B1/IN/OUT from
   the final screen. All checks should remain PASS.

This package adds a pin-inert protocol channel modeled on the reference firmware's QUERY_FW_INFO
(0xA1) and PING (0xFE) commands. It verifies the SDK application, OS service table, AXI MMIO window,
and RTL command handler end to end with an EMPTY slot. PING returns the bitwise complement of the
test value. GBA ROM-read capability stays disabled and the write path remains structurally absent.
This digital test does not prove cartridge rail voltage or close the electrical measurement gate.
Do not insert a cartridge.

Slots 1-3 have fixed filenames in data.json so the SDK OS and application still load when APF skips
slot 0 for Play Cartridge mode.

Do not use smoke.1 or smoke.2. smoke.2 was built with an uninitialized embedded boot ROM.
"@
[System.IO.File]::WriteAllText((Join-Path $bundleRoot 'README_TEST.txt'), $install,
    (New-Object System.Text.UTF8Encoding($false)))

$checksumLines = Get-ChildItem -LiteralPath $sdRoot -File -Recurse |
    Sort-Object FullName |
    ForEach-Object {
        $relative = [System.IO.Path]::GetRelativePath($sdRoot, $_.FullName).Replace('\', '/')
        $hash = (Get-FileHash -Algorithm SHA256 -LiteralPath $_.FullName).Hash.ToLowerInvariant()
        "$hash  $relative"
    }
[System.IO.File]::WriteAllLines((Join-Path $bundleRoot 'SHA256SUMS.txt'),
    [string[]]$checksumLines, (New-Object System.Text.UTF8Encoding($false)))

$zipPath = "$bundleRoot.zip"
if (Test-Path -LiteralPath $zipPath) {
    Remove-Item -LiteralPath $zipPath -Force
}
Compress-Archive -Path (Join-Path $bundleRoot '*') -DestinationPath $zipPath -CompressionLevel Optimal

Write-Host "Protocol smoke package: $zipPath"
