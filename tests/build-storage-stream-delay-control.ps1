[CmdletBinding()]
param(
    [string]$Distro = 'Ubuntu-24.04',
    [string]$OpenFpgaSdk = '/home/lin13/work/ONS_FPGA/openfpgaSDK',
    [string]$Version = '0.1.0-smoke.40-app-delay'
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
$buildRoot = Join-Path $projectRoot 'build\smoke'
$baseName = 'ChisBurner-storage-stream-0.1.0-smoke.31-flush-level'
$baseRoot = Join-Path $buildRoot $baseName
$baseSdRoot = Join-Path $baseRoot 'SD_ROOT'
$appRoot = Join-Path $projectRoot 'app\storage_stream_probe'
$appElf = Join-Path $projectRoot 'build\storage-stream-app\app.elf'

$fixed = @{
    'Assets\chisburner\common\os.bin' =
        'e87121ba586e2a43eb99aa522e055e678a964199e1802968c11ef06d76dfebaa'
    'Cores\ChisBurner.ChisBurner\chis.rbf_r' =
        'b3c3d8b013859b555dce81cf556c342b0ab8ccf460418d9b4b32f1642784dd4b'
}

foreach ($relative in $fixed.Keys) {
    $path = Join-Path $baseSdRoot $relative
    if (-not (Test-Path -LiteralPath $path)) {
        throw "Missing smoke.31 fixed input: $path"
    }
    $actual = (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash.ToLowerInvariant()
    if ($actual -ne $fixed[$relative]) {
        throw "Smoke.31 fixed input hash mismatch: $relative"
    }
}

$source = Get-Content -Raw -LiteralPath (Join-Path $appRoot 'main.c')
foreach ($required in @(
        'TF STORAGE smoke.40',
        '/Saves/chisburner/common/storage-smoke.40.bin',
        'stage("SETTLE 2S", offset);',
        'usleep(2000000);')) {
    if (-not $source.Contains($required)) {
        throw "Smoke.40 application marker is missing: $required"
    }
}
if ($source -match 'of_cart_') {
    throw 'The TF storage diagnostic must not call a cartridge service'
}

$appWsl = Convert-ToWslPath $appRoot
Invoke-WslBash "cd '$appWsl' && make clean OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0 && make OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0"
if (-not (Test-Path -LiteralPath $appElf)) {
    throw "Storage application build did not produce $appElf"
}

$packageName = "ChisBurner-storage-stream-$Version"
$bundleRoot = Join-Path $buildRoot $packageName
$bundleFull = [System.IO.Path]::GetFullPath($bundleRoot)
$buildFull = [System.IO.Path]::GetFullPath($buildRoot).TrimEnd('\') + '\'
if (-not $bundleFull.StartsWith(
        $buildFull, [System.StringComparison]::OrdinalIgnoreCase)) {
    throw "Refusing to replace a bundle outside the smoke root: $bundleFull"
}
if (Test-Path -LiteralPath $bundleFull) {
    Remove-Item -LiteralPath $bundleFull -Recurse -Force
}
New-Item -ItemType Directory -Force $bundleRoot | Out-Null
Copy-Item -LiteralPath $baseSdRoot -Destination $bundleRoot -Recurse

$sdRoot = Join-Path $bundleRoot 'SD_ROOT'
$coreRoot = Join-Path $sdRoot 'Cores\ChisBurner.ChisBurner'
$commonAssets = Join-Path $sdRoot 'Assets\chisburner\common'
$instanceRoot = Join-Path $sdRoot 'Assets\chisburner\ChisBurner.ChisBurner'
$saveRoot = Join-Path $sdRoot 'Saves\chisburner\common'

Copy-Item -LiteralPath $appElf `
    -Destination (Join-Path $commonAssets 'chisburnersmoke.elf') -Force

$oldInstance = Join-Path $instanceRoot 'TF Storage Stream.json'
$newInstance = Join-Path $instanceRoot 'TF Storage DELAY 40.json'
if (-not (Test-Path -LiteralPath $oldInstance)) {
    throw "Missing base instance descriptor: $oldInstance"
}
Move-Item -LiteralPath $oldInstance -Destination $newInstance

New-Item -ItemType Directory -Force $saveRoot | Out-Null
[System.IO.File]::WriteAllBytes(
    (Join-Path $saveRoot 'storage-smoke.40.bin'), [byte[]](0))

$coreJsonPath = Join-Path $coreRoot 'core.json'
$coreJson = Get-Content -Raw -LiteralPath $coreJsonPath | ConvertFrom-Json
$coreJson.core.metadata.version = $Version
$coreJson.core.metadata.date_release = '2026-09-03'
[System.IO.File]::WriteAllText(
    $coreJsonPath, ($coreJson | ConvertTo-Json -Depth 20),
    (New-Object System.Text.UTF8Encoding($false)))

foreach ($relative in $fixed.Keys) {
    $path = Join-Path $sdRoot $relative
    $actual = (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash.ToLowerInvariant()
    if ($actual -ne $fixed[$relative]) {
        throw "Fixed smoke.31 artifact changed unexpectedly: $relative"
    }
}

$appHash = (Get-FileHash -LiteralPath `
    (Join-Path $commonAssets 'chisburnersmoke.elf') -Algorithm SHA256).Hash.ToLowerInvariant()
$baseAppHash = (Get-FileHash -LiteralPath `
    (Join-Path $baseSdRoot 'Assets\chisburner\common\chisburnersmoke.elf') `
    -Algorithm SHA256).Hash.ToLowerInvariant()
if ($appHash -eq $baseAppHash) {
    throw 'Smoke.40 application unexpectedly matches the smoke.31 application'
}

$readme = @"
ChisBurner - application-only delay test $Version

Run with the cartridge slot EMPTY.
Merge the complete SD_ROOT into the SD-card root and approve every replacement.
Select "TF Storage DELAY 40".

The screen must say "TF STORAGE smoke.40". If it shows an older number, delete
the stale ChisBurner core/asset directories and merge the complete SD_ROOT again.

This package keeps smoke.31 os.bin and chis.rbf_r byte-for-byte unchanged.
Only the application changes: after the final write it displays "SETTLE 2S",
waits two seconds, and then calls FLUSH once.

Report the final FLUSH line and any bracketed timeout line above it.
This package does not request cartridge access.
"@
[System.IO.File]::WriteAllText(
    (Join-Path $bundleRoot 'README_TEST.txt'), $readme,
    (New-Object System.Text.UTF8Encoding($false)))

$provenance = @"
version=$Version
variant=smoke31_os_rbf_app_delay_only
rbf_sha256=$($fixed['Cores\ChisBurner.ChisBurner\chis.rbf_r'])
os_sha256=$($fixed['Assets\chisburner\common\os.bin'])
app_sha256=$appHash
base_app_sha256=$baseAppHash
fixed_os_rbf_byte_identical_smoke31=PASS
application_change=2_second_settle_before_flush
"@
[System.IO.File]::WriteAllText(
    (Join-Path $bundleRoot 'BUILD_PROVENANCE.txt'), $provenance,
    (New-Object System.Text.UTF8Encoding($false)))

$prefix = [System.IO.Path]::GetFullPath($sdRoot).TrimEnd('\') + '\'
$sumLines = Get-ChildItem -LiteralPath $sdRoot -File -Recurse |
    Sort-Object FullName | ForEach-Object {
        $relative = $_.FullName.Substring($prefix.Length).Replace('\', '/')
        $hash = (Get-FileHash -LiteralPath $_.FullName `
            -Algorithm SHA256).Hash.ToLowerInvariant()
        "$hash  $relative"
    }
[System.IO.File]::WriteAllLines(
    (Join-Path $bundleRoot 'SHA256SUMS.txt'), [string[]]$sumLines,
    (New-Object System.Text.UTF8Encoding($false)))

$zipPath = "$bundleRoot.zip"
if (Test-Path -LiteralPath $zipPath) {
    Remove-Item -LiteralPath $zipPath -Force
}
Compress-Archive -Path (Join-Path $bundleRoot '*') `
    -DestinationPath $zipPath -CompressionLevel Optimal

Write-Output "Application-only delay package: $zipPath"
