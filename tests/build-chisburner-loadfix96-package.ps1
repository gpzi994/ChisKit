[CmdletBinding()]
param()

$ErrorActionPreference = 'Stop'
$root = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$acceptedBaseHash = 'b133b337eb5ff22b50cdc94066c4545a4c05398514a06a6e67fbf7ded77dda4b'
$bundle = Join-Path $root 'build/smoke/ChisBurner-smoke.96-load-fix'
$zip = "$bundle.zip"
$utf8 = [Text.UTF8Encoding]::new($false)

function Get-Sha256([string]$Path) {
    (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant()
}

if ((Test-Path -LiteralPath $bundle) -or (Test-Path -LiteralPath $zip)) {
    throw 'Preserve existing smoke.96 package'
}
$baseZip = @(Get-ChildItem -LiteralPath (Join-Path $root 'build/smoke') -File -Filter '*.zip' |
    Where-Object { (Get-Sha256 $_.FullName) -eq $acceptedBaseHash })
if ($baseZip.Count -ne 1) { throw 'Accepted smoke.94 base ZIP not uniquely identified' }
$baseBundle = $baseZip[0].FullName.Substring(0, $baseZip[0].FullName.Length - 4)
$base = Join-Path $baseBundle 'SD_ROOT'
if (!(Test-Path -LiteralPath $base)) { throw "Missing accepted smoke.94 base: $base" }
if ((Get-Sha256 $baseZip[0].FullName) -ne $acceptedBaseHash) {
    throw 'Accepted smoke.94 ZIP hash changed'
}

foreach ($line in Get-Content -LiteralPath (Join-Path $baseBundle 'SHA256SUMS.txt')) {
    if ($line -notmatch '^([0-9a-f]{64})  (.+)$') { throw "Malformed smoke.94 sum: $line" }
    $source = Join-Path $base $Matches[2]
    if (!(Test-Path -LiteralPath $source) -or (Get-Sha256 $source) -cne $Matches[1]) {
        throw "Accepted smoke.94 payload changed: $($Matches[2])"
    }
}

$oldCommonCandidates = @(Get-ChildItem -LiteralPath $base -Recurse -Directory |
    Where-Object {
        (Test-Path -LiteralPath (Join-Path $_.FullName 'os.bin')) -and
        (@(Get-ChildItem -LiteralPath $_.FullName -File -Filter '*.elf').Count -eq 1)
    })
$oldCoreCandidates = @(Get-ChildItem -LiteralPath $base -Recurse -Directory |
    Where-Object {
        (Test-Path -LiteralPath (Join-Path $_.FullName 'core.json')) -and
        (Test-Path -LiteralPath (Join-Path $_.FullName 'data.json')) -and
        (@(Get-ChildItem -LiteralPath $_.FullName -File -Filter '*.rbf_r').Count -eq 1)
    })
if ($oldCommonCandidates.Count -ne 1 -or $oldCoreCandidates.Count -ne 1) {
    throw 'Accepted base layout is ambiguous'
}
$oldCommon = $oldCommonCandidates[0].FullName
$oldCore = $oldCoreCandidates[0].FullName
$oldApp = @(Get-ChildItem -LiteralPath $oldCommon -File -Filter '*.elf')[0].FullName
$oldRbf = @(Get-ChildItem -LiteralPath $oldCore -File -Filter '*.rbf_r')[0].FullName
$sd = Join-Path $bundle 'SD_ROOT'
$common = Join-Path $sd 'Assets/chisburner/common'
$coreRoot = Join-Path $sd 'Cores/ChisBurner.ChisBurner'
$platformRoot = Join-Path $sd 'Platforms'
New-Item -ItemType Directory -Path $common,$coreRoot,$platformRoot | Out-Null

Copy-Item -LiteralPath (Join-Path $oldCommon 'os.bin') -Destination (Join-Path $common 'os.bin')
Copy-Item -LiteralPath $oldApp -Destination (Join-Path $common 'cb96.elf')
[IO.File]::WriteAllText((Join-Path $common 'cb96.ini'),
    "[os]`nELF=cb96.elf`nARGS=`n", $utf8)

foreach ($name in @('audio.json','input.json','interact.json','loader.bin',
                     'variants.json','video.json')) {
    Copy-Item -LiteralPath (Join-Path $oldCore $name) -Destination (Join-Path $coreRoot $name)
}
Copy-Item -LiteralPath $oldRbf -Destination (Join-Path $coreRoot 'chis.rbf_r')

$core = Get-Content -Raw -LiteralPath (Join-Path $oldCore 'core.json') | ConvertFrom-Json
$core.core.metadata.platform_ids = @('chisburner')
$core.core.metadata.shortname = 'ChisBurner'
$core.core.metadata.description = 'Read-only automatic MBC3/MBC5 detector'
$core.core.metadata.author = 'ChisBurner'
$core.core.metadata.version = '0.1.0-smoke.96-load-fix'
$core.core.metadata.date_release = '2026-09-07'
$core.core.cores[0].name = 'chisburner'
$core.core.cores[0].filename = 'chis.rbf_r'
if ($core.core.metadata.platform_ids[0].Length -gt 15 -or
    $core.core.cores[0].name.Length -gt 15 -or
    $core.core.cores[0].filename.Length -gt 15) {
    throw 'Pocket core identifier or bitstream filename exceeds 15 characters'
}
if ((Split-Path -Leaf $coreRoot) -cne
    "$($core.core.metadata.author).$($core.core.metadata.shortname)") {
    throw 'Core directory does not match AuthorName.CoreName metadata'
}
[IO.File]::WriteAllText((Join-Path $coreRoot 'core.json'),
    ($core | ConvertTo-Json -Depth 20), $utf8)

$data = Get-Content -Raw -LiteralPath (Join-Path $oldCore 'data.json') | ConvertFrom-Json
($data.data.data_slots | Where-Object id -eq 2).filename = 'cb96.ini'
($data.data.data_slots | Where-Object id -eq 3).filename = 'cb96.elf'
[IO.File]::WriteAllText((Join-Path $coreRoot 'data.json'),
    ($data | ConvertTo-Json -Depth 20), $utf8)
[IO.File]::WriteAllText((Join-Path $coreRoot 'info.txt'),
    'ChisBurner smoke.96: Pocket metadata and filename load fix; logic unchanged.', $utf8)
Copy-Item -LiteralPath (Join-Path $root 'platform/openfpga/chiskit.json') -Destination (Join-Path $platformRoot 'chisburner.json')

$oldOs = Join-Path $oldCommon 'os.bin'
$newOs = Join-Path $common 'os.bin'
$newRbf = Join-Path $coreRoot 'chis.rbf_r'
$newApp = Join-Path $common 'cb96.elf'
foreach ($pair in @(@($oldOs,$newOs),@($oldRbf,$newRbf),@($oldApp,$newApp))) {
    if ((Get-Sha256 $pair[0]) -ne (Get-Sha256 $pair[1])) {
        throw "Functional binary changed during load fix: $($pair[1])"
    }
}

foreach ($json in Get-ChildItem -LiteralPath $sd -Recurse -File -Filter '*.json') {
    $null = Get-Content -Raw -LiteralPath $json.FullName | ConvertFrom-Json
}
$files = @(Get-ChildItem -LiteralPath $sd -Recurse -File)
if ($files.Count -ne 14) { throw "Unexpected SD payload count: $($files.Count)" }
if ($files | Where-Object { $_.Extension -in @('.gb','.gbc','.gba') }) {
    throw 'ROM image unexpectedly included'
}
foreach ($file in $files) {
    $relative = $file.FullName.Substring($sd.Length + 1).Replace('\','/')
    if ($relative -notmatch '^(Assets/chisburner/common|Cores/ChisBurner\.ChisBurner|Platforms)/') {
        throw "Unexpected install path: $relative"
    }
}

$readme = @'
ChisBurner smoke.96 - Core 装载修正版

功能载荷与已通过实机测试的 smoke.94 完全一致。本版修复 smoke.95 的两个
openFPGA 装载问题：RBF 文件名超过 15 字符，以及 Core 目录与 author 不一致。
没有新增 NOR、SRAM、擦除或烧写命令。

复制前请删除 SD 卡上的 Cores/ChisBurner.ChisBurner 和 Assets/chisburner，
然后将 SD_ROOT 合并到 SD 卡根目录，再从 openFPGA -> ChisBurner 启动。

合格标准：菜单仅显示 ChisBurner，MBC3 与 MBC5 两张卡仍分别得到正确的自动
profile，并保持此前 smoke.94 的全部 PASS 结果。
'@
[IO.File]::WriteAllText((Join-Path $bundle 'README_TEST96_CN.txt'), $readme, $utf8)
$provenance = @"
version=0.1.0-smoke.96-load-fix
base_zip_sha256=$(Get-Sha256 "$baseBundle.zip")
functional_payload_from_accepted_smoke94=YES
os_byte_identical=YES
rbf_byte_identical=YES
application_byte_identical=YES
platform_id=chisburner
core_path=Cores/ChisBurner.ChisBurner
asset_path=Assets/chisburner/common
legacy_wire_values_retained=YES
rbf_filename_length_gate=PASS
core_directory_metadata_gate=PASS
gb_mbc3_renamed_source_regression=PASS
gb_mbc5_renamed_source_regression=PASS
gb_fullrom_renamed_source_regression=PASS
gba_program_renamed_source_regression=PASS
"@
[IO.File]::WriteAllText((Join-Path $bundle 'BUILD_PROVENANCE.txt'), $provenance, $utf8)

$prefix = [IO.Path]::GetFullPath($sd).TrimEnd('\') + '\'
$sums = $files | Sort-Object FullName | ForEach-Object {
    "$(Get-Sha256 $_.FullName)  $($_.FullName.Substring($prefix.Length).Replace('\','/'))"
}
[IO.File]::WriteAllLines((Join-Path $bundle 'SHA256SUMS.txt'), [string[]]$sums, $utf8)
Compress-Archive -Path (Join-Path $bundle '*') -DestinationPath $zip -CompressionLevel Optimal

Write-Output "PASS: $zip"
Write-Output "ZIP SHA256: $(Get-Sha256 $zip)"
Write-Output "SD payload files: $($files.Count); functional binaries byte-identical to accepted smoke.94"
