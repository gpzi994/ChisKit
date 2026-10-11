[CmdletBinding()]
param(
    [string]$Distro = 'Ubuntu-24.04',
    [string]$OpenFpgaSdk = '/home/lin13/work/ONS_FPGA/openfpgaSDK',
    [string]$Version = '0.1.0-smoke.41-crc-rebuild'
)

$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'os-image-integrity.ps1')
if ($Version -eq '0.1.0-smoke.41-size-commit') {
    throw 'Preserve the original smoke.41 CRC-defect package as diagnostic evidence'
}

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
$baseSdRoot = Join-Path (Join-Path $buildRoot $baseName) 'SD_ROOT'
$baseOs = Join-Path $baseSdRoot 'Assets\chisburner\common\os.bin'
$baseRbf = Join-Path $baseSdRoot `
    'Cores\ChisBurner.ChisBurner\chis.rbf_r'
$appRoot = Join-Path $projectRoot 'app\storage_stream_probe'
$appElf = Join-Path $projectRoot 'build\storage-stream-app\app.elf'
$patchSource = Join-Path $projectRoot `
    'tests\patches\smoke31-output-size-commit.S'
$patchBuild = Join-Path $projectRoot 'build\smoke31-output-size-patch'
$patchObject = Join-Path $patchBuild 'patch.o'
$patchBinary = Join-Path $patchBuild 'patch.bin'

$baseOsHash = 'e87121ba586e2a43eb99aa522e055e678a964199e1802968c11ef06d76dfebaa'
$baseRbfHash = 'b3c3d8b013859b555dce81cf556c342b0ab8ccf460418d9b4b32f1642784dd4b'
foreach ($pair in @(@($baseOs, $baseOsHash), @($baseRbf, $baseRbfHash))) {
    if (-not (Test-Path -LiteralPath $pair[0])) {
        throw "Missing smoke.31 input: $($pair[0])"
    }
    $actual = (Get-FileHash -LiteralPath $pair[0] `
        -Algorithm SHA256).Hash.ToLowerInvariant()
    if ($actual -ne $pair[1]) {
        throw "Smoke.31 input hash mismatch: $($pair[0])"
    }
}

$source = Get-Content -Raw -LiteralPath (Join-Path $appRoot 'main.c')
foreach ($required in @(
        'TF STORAGE smoke.41',
        '/Saves/chisburner/common/storage-smoke.41.bin')) {
    if (-not $source.Contains($required)) {
        throw "Smoke.41 application marker is missing: $required"
    }
}
if ($source -match 'of_cart_' -or $source -match '0x400000') {
    throw 'The application must use services only and contain no cartridge or MMIO access'
}

$appWsl = Convert-ToWslPath $appRoot
Invoke-WslBash "cd '$appWsl' && make clean OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0 && make OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0"
if (-not (Test-Path -LiteralPath $appElf)) {
    throw "Storage application build did not produce $appElf"
}

New-Item -ItemType Directory -Force $patchBuild | Out-Null
$patchSourceWsl = Convert-ToWslPath $patchSource
$patchObjectWsl = Convert-ToWslPath $patchObject
$patchBinaryWsl = Convert-ToWslPath $patchBinary
Invoke-WslBash "riscv64-unknown-elf-as -march=rv32imafc -mabi=ilp32f -o '$patchObjectWsl' '$patchSourceWsl' && riscv64-unknown-elf-objcopy -O binary -j .text '$patchObjectWsl' '$patchBinaryWsl'"

$patchBytes = [System.IO.File]::ReadAllBytes($patchBinary)
if ($patchBytes.Length -ne 110) {
    throw "Unexpected fixed-layout patch size: $($patchBytes.Length)"
}

$osBytes = [System.IO.File]::ReadAllBytes($baseOs)
$null = Assert-ChisBurnerOsIntegrity -Bytes $osBytes -Context 'Smoke.31 source OS'
$originalBytes = [byte[]]$osBytes.Clone()
$hookOffset = 0xF392
$caveOffset = 0xF3EC
$patchCaveOffset = 90
$expectedHook = [byte[]](0xB7, 0xE7, 0x38, 0x10)
$expectedNop = [byte[]](0x13, 0x00, 0x00, 0x00)
for ($i = 0; $i -lt $expectedHook.Length; $i++) {
    if ($osBytes[$hookOffset + $i] -ne $expectedHook[$i]) {
        throw 'Smoke.31 output-open hook signature mismatch'
    }
}
for ($i = 0; $i -lt 20; $i++) {
    if ($osBytes[$caveOffset + $i] -ne $expectedNop[$i % 4]) {
        throw 'Smoke.31 output-open alignment cave signature mismatch'
    }
}

[Array]::Copy($patchBytes, 0, $osBytes, $hookOffset, 4)
[Array]::Copy($patchBytes, $patchCaveOffset, $osBytes, $caveOffset, 20)

$changed = New-Object System.Collections.Generic.List[int]
for ($i = 0; $i -lt $osBytes.Length; $i++) {
    if ($osBytes[$i] -ne $originalBytes[$i]) {
        $changed.Add($i)
        if (-not (($i -ge $hookOffset -and $i -lt $hookOffset + 4) -or
                  ($i -ge $caveOffset -and $i -lt $caveOffset + 20))) {
            throw "Patch changed an out-of-scope byte at offset 0x$($i.ToString('X'))"
        }
    }
}
if ($changed.Count -lt 16 -or $changed.Count -gt 24) {
    throw "Unexpected changed-byte count: $($changed.Count)"
}

# Code changes invalidate the internal OFC1 CRC even when length, metadata
# and all other offsets are preserved. Package SHA256 is not a substitute.
$osBytes = Set-ChisBurnerOsCrc -Bytes $osBytes
$crcInfo = Assert-ChisBurnerOsIntegrity -Bytes $osBytes -Context 'Patched OS'

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
$instanceRoot = Join-Path $sdRoot `
    'Assets\chisburner\ChisBurner.ChisBurner'
$saveRoot = Join-Path $sdRoot 'Saves\chisburner\common'
$patchedOs = Join-Path $commonAssets 'os.bin'
[System.IO.File]::WriteAllBytes($patchedOs, $osBytes)
Copy-Item -LiteralPath $appElf `
    -Destination (Join-Path $commonAssets 'chisburnersmoke.elf') -Force

$oldInstance = Join-Path $instanceRoot 'TF Storage Stream.json'
$newInstance = Join-Path $instanceRoot 'TF Storage SIZE 41.json'
if (-not (Test-Path -LiteralPath $oldInstance)) {
    throw "Missing base instance descriptor: $oldInstance"
}
Move-Item -LiteralPath $oldInstance -Destination $newInstance

New-Item -ItemType Directory -Force $saveRoot | Out-Null
[System.IO.File]::WriteAllBytes(
    (Join-Path $saveRoot 'storage-smoke.41.bin'), [byte[]](0))

$coreJsonPath = Join-Path $coreRoot 'core.json'
$coreJson = Get-Content -Raw -LiteralPath $coreJsonPath | ConvertFrom-Json
$coreJson.core.metadata.version = $Version
$coreJson.core.metadata.date_release = '2026-09-03'
[System.IO.File]::WriteAllText(
    $coreJsonPath, ($coreJson | ConvertTo-Json -Depth 20),
    (New-Object System.Text.UTF8Encoding($false)))

$packagedRbfHash = (Get-FileHash -LiteralPath `
    (Join-Path $coreRoot 'chis.rbf_r') `
    -Algorithm SHA256).Hash.ToLowerInvariant()
if ($packagedRbfHash -ne $baseRbfHash) {
    throw 'Packaged RBF changed unexpectedly'
}
if ((Get-Item -LiteralPath $patchedOs).Length -ne
        (Get-Item -LiteralPath $baseOs).Length) {
    throw 'Patched OS length changed unexpectedly'
}

$patchedOsHash = (Get-FileHash -LiteralPath $patchedOs `
    -Algorithm SHA256).Hash.ToLowerInvariant()
$appHash = (Get-FileHash -LiteralPath `
    (Join-Path $commonAssets 'chisburnersmoke.elf') `
    -Algorithm SHA256).Hash.ToLowerInvariant()

$readme = @"
ChisBurner - output-size commit test $Version

Run with the cartridge slot EMPTY.
Merge the complete SD_ROOT into the SD-card root and approve every replacement.
Select "TF Storage SIZE 41".

The screen must say "TF STORAGE smoke.41". If it shows an older number, delete
the stale ChisBurner core/asset directories and merge the complete SD_ROOT again.

The RBF is byte-for-byte smoke.31. The OS is the proven smoke.31 binary with a
source-guided fixed-layout patch: 24 bytes are reserved/changed, its length and
all startup addresses remain unchanged. After output slot 4 opens, it commits
the requested size into datatable entry 4 (word 9) before normal writes/FLUSH.

The OFC1 internal CRC is regenerated after patching and validated in the ZIP.
FLUSH recovery is not established. Report its result and any startup warning.
This package does not request cartridge access.
"@
[System.IO.File]::WriteAllText(
    (Join-Path $bundleRoot 'README_TEST.txt'), $readme,
    (New-Object System.Text.UTF8Encoding($false)))

$provenance = @"
version=$Version
variant=smoke31_fixed_layout_output_size_commit
rbf_sha256=$baseRbfHash
base_os_sha256=$baseOsHash
patched_os_sha256=$patchedOsHash
app_sha256=$appHash
os_length=$($osBytes.Length)
changed_code_byte_count=$($changed.Count)
changed_regions=0xF392-0xF395,0xF3EC-0xF3FF,final_four_crc_bytes
internal_crc=$('{0:x8}' -f $crcInfo.ComputedCrc)
internal_crc_valid=PASS
source_patch=tests/patches/smoke31-output-size-commit.S
startup_layout_unchanged=PASS
application_mmio=NONE
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
$null = Assert-ChisBurnerZipOsIntegrity -ZipPath $zipPath

Write-Output "Output-size commit package: $zipPath"
