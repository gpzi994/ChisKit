[CmdletBinding()]
param(
    [string]$Distro = 'Ubuntu-24.04',
    [string]$OpenFpgaSdk = '/home/lin13/work/ONS_FPGA/openfpgaSDK',
    [string]$Version = '0.1.0-smoke.38-fixed-layout-trace'
)

$ErrorActionPreference = 'Stop'

function Convert-ToWslPath([string]$Path) {
    $full = [System.IO.Path]::GetFullPath($Path)
    if ($full -notmatch '^([A-Za-z]):\\(.*)$') {
        throw "Expected a local Windows drive path: $full"
    }
    return '/mnt/' + $Matches[1].ToLowerInvariant() + '/' +
        $Matches[2].Replace('\', '/')
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
$baseRbf = Join-Path $baseSdRoot `
    'Cores\ChisBurner.ChisBurner\chis.rbf_r'
$baseOs = Join-Path $baseSdRoot 'Assets\chisburner\common\os.bin'
$baseSums = Join-Path $baseRoot 'SHA256SUMS.txt'
$baseProvenance = Join-Path $baseRoot 'BUILD_PROVENANCE.txt'
$currentOs = Join-Path $projectRoot 'build\storage-stream-artifacts\os.bin'
$appRoot = Join-Path $projectRoot 'app\storage_stream_probe'
$appElf = Join-Path $projectRoot 'build\storage-stream-app\app.elf'

foreach ($required in @($baseSdRoot, $baseRbf, $baseOs, $baseSums,
        $baseProvenance, $currentOs, $appRoot)) {
    if (-not (Test-Path -LiteralPath $required)) {
        throw "Required isolation-build input is missing: $required"
    }
}

$provenanceText = Get-Content -Raw -LiteralPath $baseProvenance
foreach ($proof in @(
        'variant=chisburner_storage_stream',
        'quartus_full_compile=PASS',
        'quartus_mif_update=PASS')) {
    if ($provenanceText -notmatch ('(?m)^' + [regex]::Escape($proof) + '$')) {
        throw "The device-proven base RBF lacks proof: $proof"
    }
}
$baseCore = Get-Content -Raw -LiteralPath (Join-Path $baseSdRoot `
    'Cores\ChisBurner.ChisBurner\core.json') | ConvertFrom-Json
if ([int64]$baseCore.core.framework.hardware.cartridge_adapter -ne -1) {
    throw 'The device-proven base RBF does not keep cartridge power off'
}
$baseRbfHash = (Get-FileHash -LiteralPath $baseRbf -Algorithm SHA256).Hash
$manifestLine = Get-Content -LiteralPath $baseSums | Where-Object {
    $_ -match 'chisburner\.rbf_r$'
}
if ($manifestLine.Count -ne 1 -or
        $manifestLine -notmatch ('(?i)^' + $baseRbfHash + '\s+')) {
    throw 'The device-proven base RBF does not match its signed manifest'
}

$baseOsBytes = [System.IO.File]::ReadAllBytes($baseOs)
$currentOsBytes = [System.IO.File]::ReadAllBytes($currentOs)
if ($currentOsBytes.Length -ne $baseOsBytes.Length -or
        $currentOsBytes.Length -ne 137480) {
    throw 'The candidate OS does not preserve the smoke.31 image length'
}
$metaOffset = $currentOsBytes.Length - 24
if ([BitConverter]::ToUInt32($currentOsBytes, $metaOffset) -ne 0x3245534F) {
    throw 'The candidate OS lacks valid OSE2 metadata'
}
$osBssStart = [BitConverter]::ToUInt32($currentOsBytes, $metaOffset + 8)
$osBssEnd = [BitConverter]::ToUInt32($currentOsBytes, $metaOffset + 12)
if ($osBssStart -ne 0x10341CF0 -or $osBssEnd -ne 0x1038E58C) {
    throw 'The candidate OS does not preserve the smoke.31 OSE2 BSS range'
}
if (-not [Text.Encoding]::ASCII.GetString($currentOsBytes).Contains(
        '[file] smoke.38 skip')) {
    throw 'The candidate OS lacks the smoke.38 pre-app deployment marker'
}

$appWsl = Convert-ToWslPath $appRoot
Invoke-WslBash "cd '$appWsl' && make OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0"
if (-not (Test-Path -LiteralPath $appElf)) {
    throw "Application build did not produce $appElf"
}
$appSource = Get-Content -Raw -LiteralPath (Join-Path $appRoot 'main.c')
if ($appSource -match 'of_cart_' -or
        $appSource -notmatch [regex]::Escape(
            '/Saves/chisburner/common/storage-smoke.38.bin')) {
    throw 'The smoke.38 application source violates its isolation contract'
}

$packageName = "ChisBurner-storage-stream-$Version"
$bundleRoot = Join-Path $buildRoot $packageName
$bundleFull = [System.IO.Path]::GetFullPath($bundleRoot)
$buildFull = [System.IO.Path]::GetFullPath($buildRoot).TrimEnd('\') + '\'
if (-not $bundleFull.StartsWith(
        $buildFull, [System.StringComparison]::OrdinalIgnoreCase)) {
    throw "Refusing to clean a bundle outside the smoke root: $bundleFull"
}
if (Test-Path -LiteralPath $bundleFull) {
    Remove-Item -LiteralPath $bundleFull -Recurse -Force
}
New-Item -ItemType Directory -Force $bundleRoot | Out-Null
Copy-Item -LiteralPath $baseSdRoot -Destination $bundleRoot -Recurse

$sdRoot = Join-Path $bundleRoot 'SD_ROOT'
$coreRoot = Join-Path $sdRoot 'Cores\ChisBurner.ChisBurner'
$commonAssets = Join-Path $sdRoot 'Assets\chisburner\common'
$saveRoot = Join-Path $sdRoot 'Saves\chisburner\common'
Copy-Item -LiteralPath $currentOs -Destination (Join-Path $commonAssets 'os.bin') -Force
Copy-Item -LiteralPath $appElf -Destination (Join-Path $commonAssets 'chisburnersmoke.elf') -Force

$coreJsonPath = Join-Path $coreRoot 'core.json'
$coreJson = Get-Content -Raw -LiteralPath $coreJsonPath | ConvertFrom-Json
$coreJson.core.metadata.version = $Version
$coreJson.core.metadata.date_release = '2026-09-03'
[System.IO.File]::WriteAllText(
    $coreJsonPath, ($coreJson | ConvertTo-Json -Depth 20),
    (New-Object System.Text.UTF8Encoding($false)))

$oldSeed = Join-Path $saveRoot 'storage-smoke.31.bin'
if (Test-Path -LiteralPath $oldSeed) {
    Remove-Item -LiteralPath $oldSeed -Force
}
[System.IO.File]::WriteAllBytes(
    (Join-Path $saveRoot 'storage-smoke.38.bin'), [byte[]](0))

$readme = @"
ChisBurner - TF storage bridge isolate $Version

Run with the cartridge slot EMPTY.
Merge the complete SD_ROOT into the SD-card root and approve every replacement.
The red boot screen must show "[file] smoke.38 skip", followed by visible
stage breadcrumbs for file, mixer, audio, save, and late initialization.
If app loading works, the application screen says "TF STORAGE smoke.38".

This isolation package uses the exact smoke.31 RBF that reached OPEN/WRITE on
the physical Pocket, combined with the smoke.38 app and a minimal data-table
size publication in os.bin. It intentionally skips the optional startup
self-read and traces the remaining HAL initialization stages on screen.

Expected result:
  FLUSH: PASS RC=0
  STORAGE STREAM: PASS
  DIAG COMPLETE

If it stops before the application, report the exact last visible [38] line.
If FLUSH fails, copy the complete bracketed timeout line.

After PASS, power Pocket off and verify:
  file: Saves/chisburner/common/storage-smoke.38.bin
  size: 1048613 bytes
  MD5: b58dc8fe81364ad8cf5e30dbf7388ec3
  SHA256: 6cefa9b055d371f28b0fd2aa3f5d1acda6426a99a59e8e9fad0ee7c0030d6900
"@
[System.IO.File]::WriteAllText(
    (Join-Path $bundleRoot 'README_TEST.txt'), $readme,
    (New-Object System.Text.UTF8Encoding($false)))

$baseRawHash = [regex]::Match(
    $provenanceText, '(?m)^rbf_sha256=([0-9a-f]{64})$').Groups[1].Value
$provenance = @"
version=$Version
variant=chisburner_storage_stream_bridge_isolate
rbf_source=physical-pocket-passed-smoke.31
rbf_raw_sha256=$baseRawHash
rbf_r_sha256=$($baseRbfHash.ToLowerInvariant())
base_quartus_full_compile=PASS
base_quartus_mif_update=PASS
os_sha256=$((Get-FileHash -LiteralPath $currentOs -Algorithm SHA256).Hash.ToLowerInvariant())
os_size=$($currentOsBytes.Length)
os_bss=$('{0:x8}' -f $osBssStart)..$('{0:x8}' -f $osBssEnd)
startup_bridge_self_read=SKIPPED_FOR_ISOLATION
hal_screen_trace=ENABLED
app_sha256=$((Get-FileHash -LiteralPath $appElf -Algorithm SHA256).Hash.ToLowerInvariant())
payload_size=1048613
payload_crc32=e3e64e77
payload_md5=b58dc8fe81364ad8cf5e30dbf7388ec3
payload_sha256=6cefa9b055d371f28b0fd2aa3f5d1acda6426a99a59e8e9fad0ee7c0030d6900
"@
[System.IO.File]::WriteAllText(
    (Join-Path $bundleRoot 'BUILD_PROVENANCE.txt'), $provenance,
    (New-Object System.Text.UTF8Encoding($false)))

$prefix = [System.IO.Path]::GetFullPath($sdRoot).TrimEnd('\') + '\'
$sumLines = Get-ChildItem -LiteralPath $sdRoot -File -Recurse |
    Sort-Object FullName | ForEach-Object {
        $rel = $_.FullName.Substring($prefix.Length).Replace('\', '/')
        $hash = (Get-FileHash -LiteralPath $_.FullName `
            -Algorithm SHA256).Hash.ToLowerInvariant()
        "$hash  $rel"
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
Write-Host "Bridge-isolate storage smoke package: $zipPath"
