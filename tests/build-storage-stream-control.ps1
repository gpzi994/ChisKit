[CmdletBinding()]
param(
    [string]$Version = '0.1.0-smoke.39-exact-smoke31-control'
)

$ErrorActionPreference = 'Stop'

$projectRoot = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$buildRoot = Join-Path $projectRoot 'build\smoke'
$baseName = 'ChisBurner-storage-stream-0.1.0-smoke.31-flush-level'
$baseRoot = Join-Path $buildRoot $baseName
$baseSdRoot = Join-Path $baseRoot 'SD_ROOT'

$critical = @{
    'Assets\chisburner\common\os.bin' =
        'e87121ba586e2a43eb99aa522e055e678a964199e1802968c11ef06d76dfebaa'
    'Assets\chisburner\common\chisburnersmoke.elf' =
        '6e05a7adf7377f8d9b6ab1de386774f37763bd593fc28b740a1c9fc860a27e03'
    'Cores\ChisBurner.ChisBurner\chis.rbf_r' =
        'b3c3d8b013859b555dce81cf556c342b0ab8ccf460418d9b4b32f1642784dd4b'
}

foreach ($relative in $critical.Keys) {
    $path = Join-Path $baseSdRoot $relative
    if (-not (Test-Path -LiteralPath $path)) {
        throw "Missing smoke.31 control input: $path"
    }
    $actual = (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash.ToLowerInvariant()
    if ($actual -ne $critical[$relative]) {
        throw "Smoke.31 control hash mismatch: $relative"
    }
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
$instanceRoot = Join-Path $sdRoot 'Assets\chisburner\ChisBurner.ChisBurner'

# Rename only the launcher-visible instance. The three executable artifacts
# remain byte-for-byte smoke.31 so this is a strict hardware control run.
$oldInstance = Join-Path $instanceRoot 'TF Storage Stream.json'
$newInstance = Join-Path $instanceRoot 'TF Storage CONTROL 39.json'
if (-not (Test-Path -LiteralPath $oldInstance)) {
    throw "Missing base instance descriptor: $oldInstance"
}
Move-Item -LiteralPath $oldInstance -Destination $newInstance

$coreJsonPath = Join-Path $coreRoot 'core.json'
$coreJson = Get-Content -Raw -LiteralPath $coreJsonPath | ConvertFrom-Json
$coreJson.core.metadata.version = $Version
$coreJson.core.metadata.date_release = '2026-09-03'
[System.IO.File]::WriteAllText(
    $coreJsonPath, ($coreJson | ConvertTo-Json -Depth 20),
    (New-Object System.Text.UTF8Encoding($false)))

foreach ($relative in $critical.Keys) {
    $path = Join-Path $sdRoot $relative
    $actual = (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash.ToLowerInvariant()
    if ($actual -ne $critical[$relative]) {
        throw "Packaged control artifact changed unexpectedly: $relative"
    }
}

$readme = @"
ChisBurner - exact smoke.31 control packaged as $Version

Run with the cartridge slot EMPTY.
Merge the complete SD_ROOT into the SD-card root and approve every replacement.
Select the launcher item named "TF Storage CONTROL 39".

This is a strict regression control. The RBF, os.bin, and chisburnersmoke.elf are
byte-for-byte identical to the physical-Pocket-tested smoke.31 package. The
application therefore intentionally displays "TF STORAGE smoke.31".

Expected control result:
  normal OS/app startup
  OPEN: PASS RC=0
  WRITE: PASS RC=0
  FLUSH: FAIL RC=-12
  ACK timeout near operation 73, status 20

If startup is black before the smoke.31 application screen, the failure is no
longer attributable to the newly rebuilt OS and the SD installation/runtime
state must be investigated before another code change.
"@
[System.IO.File]::WriteAllText(
    (Join-Path $bundleRoot 'README_TEST.txt'), $readme,
    (New-Object System.Text.UTF8Encoding($false)))

$provenance = @"
version=$Version
variant=exact_smoke31_control
rbf_sha256=$($critical['Cores\ChisBurner.ChisBurner\chis.rbf_r'])
os_sha256=$($critical['Assets\chisburner\common\os.bin'])
app_sha256=$($critical['Assets\chisburner\common\chisburnersmoke.elf'])
critical_artifacts_byte_identical_smoke31=PASS
source_rebuild=NO
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

Write-Output "Exact smoke.31 control package: $zipPath"
