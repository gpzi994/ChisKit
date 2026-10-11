[CmdletBinding()]
param(
    [string]$Distro = 'Ubuntu-24.04',
    [string]$OpenFpgaSdk = '/home/lin13/work/ONS_FPGA/openfpgaSDK',
    [string]$Version = '0.1.0-smoke.6'
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
$appRoot = Join-Path $projectRoot 'app\sdk_baseline_smoke'
$platformRoot = Join-Path $projectRoot 'platform\openfpga'
$buildRoot = Join-Path $projectRoot 'build\smoke'
$packageName = "ChisBurner-sdk-baseline-$Version"
$bundleRoot = Join-Path $buildRoot $packageName
$sdRoot = Join-Path $bundleRoot 'SD_ROOT'
$coreId = 'ChisBurner.ChisBurner'
$coreRoot = Join-Path (Join-Path $sdRoot 'Cores') $coreId
$commonAssets = Join-Path $sdRoot 'Assets\chisburner\common'
$instanceAssets = Join-Path (Join-Path $sdRoot 'Assets\chisburner') $coreId
$platforms = Join-Path $sdRoot 'Platforms'

$sdkTail = $OpenFpgaSdk.TrimStart('/').Replace('/', '\')
$sdkWindows = "\\wsl.localhost\$Distro\$sdkTail"
$sdkRuntime = Join-Path $sdkWindows 'runtime\pocket'
$sdkTemplateRoot = Join-Path $sdkWindows 'src\sdk\platforms\pocket\templates'
$sdkRbf = Join-Path $sdkRuntime 'os25.rbf_r'
$sdkLoader = Join-Path $sdkRuntime 'loader.bin'
$sdkOs = Join-Path $sdkRuntime 'os.bin'

foreach ($required in @($sdkRbf, $sdkLoader, $sdkOs, $sdkTemplateRoot,
                         (Join-Path $platformRoot 'core-sdk-baseline.json'),
                         (Join-Path $platformRoot 'info-sdk-baseline.txt'))) {
    if (-not (Test-Path -LiteralPath $required)) {
        throw "Required SDK baseline artifact is missing: $required"
    }
}

$buildRootFull = [System.IO.Path]::GetFullPath($buildRoot)
$bundleRootFull = [System.IO.Path]::GetFullPath($bundleRoot)
if (-not $bundleRootFull.StartsWith($buildRootFull + [System.IO.Path]::DirectorySeparatorChar,
                                    [System.StringComparison]::OrdinalIgnoreCase)) {
    throw "Refusing to clean outside the smoke build root: $bundleRootFull"
}
if (Test-Path -LiteralPath $bundleRootFull) {
    Remove-Item -LiteralPath $bundleRootFull -Recurse -Force
}

$appWsl = Convert-ToWslPath $appRoot
Invoke-WslBash "cd '$appWsl' && make OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0"
$appElf = Join-Path $projectRoot 'build\sdk-baseline-app\app.elf'
if (-not (Test-Path -LiteralPath $appElf)) {
    throw "SDK baseline application build did not produce $appElf"
}

New-Item -ItemType Directory -Force $coreRoot, $commonAssets, $instanceAssets, $platforms | Out-Null
Copy-Item -LiteralPath (Join-Path $platformRoot 'core-sdk-baseline.json') -Destination (Join-Path $coreRoot 'core.json')
foreach ($name in @('data.json', 'video.json', 'audio.json', 'input.json', 'variants.json')) {
    Copy-Item -LiteralPath (Join-Path $sdkTemplateRoot $name) -Destination $coreRoot
}
Copy-Item -LiteralPath (Join-Path $platformRoot 'interact.json') -Destination $coreRoot
Copy-Item -LiteralPath (Join-Path $platformRoot 'info-sdk-baseline.txt') `
    -Destination (Join-Path $coreRoot 'info.txt')
Copy-Item -LiteralPath $sdkLoader -Destination (Join-Path $coreRoot 'loader.bin')
Copy-Item -LiteralPath $sdkRbf -Destination (Join-Path $coreRoot 'os25.rbf_r')
Copy-Item -LiteralPath $sdkOs -Destination (Join-Path $commonAssets 'os.bin')
Copy-Item -LiteralPath $appElf -Destination (Join-Path $commonAssets 'chisburnersmoke.elf')
Copy-Item -LiteralPath (Join-Path $appRoot 'chisburnerbase.ini') -Destination (Join-Path $commonAssets 'chisburnersmoke.ini')
Copy-Item -LiteralPath (Join-Path $platformRoot 'sdk-baseline-smoke-instance.json') `
    -Destination (Join-Path $instanceAssets 'Safe Idle Smoke.json')
Copy-Item -LiteralPath (Join-Path $platformRoot 'chisburner.json') -Destination $platforms

$core = Get-Content -Raw -LiteralPath (Join-Path $coreRoot 'core.json') | ConvertFrom-Json
if ($core.core.metadata.version -ne $Version) { throw 'SDK baseline version mismatch' }
$metadataCoreId = "$($core.core.metadata.author).$($core.core.metadata.shortname)"
if ($metadataCoreId -cne $coreId) {
    throw "Core directory identity mismatch: metadata requires '$metadataCoreId', package uses '$coreId'"
}
if ((Split-Path -Leaf $coreRoot) -cne $metadataCoreId -or
    (Split-Path -Leaf $instanceAssets) -cne $metadataCoreId) {
    throw 'Core and instance directories must exactly match metadata author.shortname'
}
if ($core.core.framework.hardware.cartridge_adapter -ne 0) { throw 'SDK baseline must match the known-good Pocket envelope' }
if ($core.core.framework.hardware.link_port -ne $true) { throw 'SDK baseline must match the known-good Pocket envelope' }
if ($core.core.cores[0].name -ne 'default') { throw 'SDK baseline core name must match the known-good Pocket envelope' }
Get-ChildItem -LiteralPath $sdRoot -Filter '*.json' -File -Recurse | ForEach-Object {
    $null = Get-Content -Raw -LiteralPath $_.FullName | ConvertFrom-Json
}

foreach ($pair in @(@($sdkRbf, (Join-Path $coreRoot 'os25.rbf_r')),
                     @($sdkLoader, (Join-Path $coreRoot 'loader.bin')),
                     @($sdkOs, (Join-Path $commonAssets 'os.bin')))) {
    if ((Get-FileHash -Algorithm SHA256 -LiteralPath $pair[0]).Hash -ne
        (Get-FileHash -Algorithm SHA256 -LiteralPath $pair[1]).Hash) {
        throw "SDK baseline artifact changed while packaging: $($pair[0])"
    }
}

$install = @"
ChisBurner - SDK baseline smoke $Version

1. Keep the cartridge slot empty.
2. Back up and remove the obsolete Cores/ChisBurner.forPocket and
   Assets/chisburner/ChisBurner.forPocket directories left by smoke.5.
3. Merge SD_ROOT into the SD-card root.
4. Open ChisBurner and select Safe Idle Smoke.
5. Expected screen: SDK BASELINE BOOT: PASS.

This diagnostic uses the installed SDK's prebuilt os25 runtime byte-for-byte
and the same APF startup envelope as the Pocket-verified ONS smoke package.
It does not call the ChisBurner cartridge service.
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
if (Test-Path -LiteralPath $zipPath) { Remove-Item -LiteralPath $zipPath -Force }
Compress-Archive -Path (Join-Path $bundleRoot '*') -DestinationPath $zipPath -CompressionLevel Optimal
Write-Host "SDK baseline smoke package: $zipPath"
