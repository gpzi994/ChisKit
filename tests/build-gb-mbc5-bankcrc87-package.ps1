[CmdletBinding()]
param(
    [string]$Distro='Ubuntu-24.04',
    [string]$OpenFpgaSdk='/home/lin13/work/ONS_FPGA/openfpgaSDK',
    [string]$RomPath='C:/Users/lin13/Desktop/game/roms/mengjiandao.gbc'
)

$ErrorActionPreference='Stop'
. (Join-Path $PSScriptRoot 'os-image-integrity.ps1')

function Sha([string]$Path){
    (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant()
}

function Wsl([string]$Command){
    & wsl.exe -d $Distro -- bash -lc $Command
    if($LASTEXITCODE -ne 0){throw "Build/test failed: $Command"}
}

$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$wroot='/mnt/'+$root.Substring(0,1).ToLowerInvariant()+'/'+$root.Substring(3).Replace('\','/')
$baseBundle=Join-Path $root 'build/smoke/ChisBurner-smoke.86-gb-mbc5-fullrom-sha256'
$base=Join-Path $baseBundle 'SD_ROOT'
$bundle=Join-Path $root 'build/smoke/ChisBurner-smoke.87-gb-mbc5-bank-crc-diagnostic'
$zip="$bundle.zip"
$rom=[IO.Path]::GetFullPath($RomPath)
$appRoot=Join-Path $root 'app/gb_mbc5_bankcrc87'
$app=Join-Path $root 'build/gb-mbc5-bankcrc87-app/app.elf'
$utf8=[Text.UTF8Encoding]::new($false)

if((Test-Path -LiteralPath $bundle) -or (Test-Path -LiteralPath $zip)){
    throw 'Preserve existing smoke.87 package'
}
foreach($path in @($baseBundle,$base,$rom,$appRoot)){
    if(!(Test-Path -LiteralPath $path)){throw "Missing package input: $path"}
}
if((Get-Item -LiteralPath $rom).Length -ne 2097152 -or
   (Sha $rom) -ne '8ba533be5908305a0f2f04117a858a91d4098ed8d4d2be6e48c3c3e9df1885e7'){
    throw 'Wrong MBC5 reference ROM'
}

$baseSums=Join-Path $baseBundle 'SHA256SUMS.txt'
foreach($line in Get-Content -LiteralPath $baseSums){
    if($line -notmatch '^([0-9a-f]{64})  (.+)$'){throw "Malformed smoke.86 sum: $line"}
    $path=Join-Path $base $Matches[2]
    if(!(Test-Path -LiteralPath $path) -or (Sha $path) -cne $Matches[1]){
        throw "smoke.86 input drift: $($Matches[2])"
    }
}

$baseOs=Join-Path $base 'Assets/chisburner/common/os.bin'
$baseRbf=Join-Path $base 'Cores/ChisBurner.ChisBurner/chis.rbf_r'
$baseOsHash=Sha $baseOs
$baseRbfHash=Sha $baseRbf
$null=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes($baseOs))

$appSource=(Get-Content -Raw -LiteralPath (Join-Path $appRoot 'main.c'))+
           (Get-Content -Raw -LiteralPath (Join-Path $appRoot 'bank_crc32.c'))
if($appSource -match 'of_cart_(write|erase|program)'){
    throw 'Bank CRC diagnostic app contains cartridge mutation call'
}

Wsl "cd '$wroot' && cc -O2 -Wall -Wextra tests/test-bankcrc87.c app/gb_mbc5_bankcrc87/bank_crc32.c -o /tmp/test-bankcrc87 && /tmp/test-bankcrc87 '/mnt/c/Users/lin13/Desktop/game/roms/mengjiandao.gbc'"
Wsl "cd '$wroot/app/gb_mbc5_bankcrc87' && make -B all OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0"
if(!(Test-Path -LiteralPath $app)){throw 'MBC5 bank CRC app build missing'}

$sd=Join-Path $bundle 'SD_ROOT'
New-Item -ItemType Directory -Path $bundle,$sd | Out-Null
$basePrefix=[IO.Path]::GetFullPath($base).TrimEnd('\')+'\'
foreach($file in Get-ChildItem -LiteralPath $base -Recurse -File){
    $rel=$file.FullName.Substring($basePrefix.Length).Replace('\','/')
    if($rel -in @('Assets/chisburner/common/chisburner86.elf',
                  'Assets/chisburner/common/chisburner86.ini')){continue}
    $dest=Join-Path $sd $rel
    New-Item -ItemType Directory -Force (Split-Path $dest -Parent) | Out-Null
    Copy-Item -LiteralPath $file.FullName -Destination $dest
}

$common=Join-Path $sd 'Assets/chisburner/common'
$coreRoot=Join-Path $sd 'Cores/ChisBurner.ChisBurner'
Copy-Item -LiteralPath $app -Destination (Join-Path $common 'chisburner87.elf')
[IO.File]::WriteAllText((Join-Path $common 'chisburner87.ini'),
    "[os]`nELF=chisburner87.elf`nARGS=`n",$utf8)

$corePath=Join-Path $coreRoot 'core.json'
$core=Get-Content -Raw -LiteralPath $corePath|ConvertFrom-Json
$core.core.metadata.description='GB/GBC MBC5 128-bank CRC32 mismatch locator'
$core.core.metadata.version='0.1.0-smoke.87-gb-mbc5-bank-crc-diagnostic'
$core.core.metadata.date_release='2026-09-06'
if($core.core.framework.hardware.cartridge_adapter -ne 16777216){
    throw 'Package must retain Play Cartridge mode'
}
[IO.File]::WriteAllText($corePath,($core|ConvertTo-Json -Depth 20),$utf8)

$dataPath=Join-Path $coreRoot 'data.json'
$data=Get-Content -Raw -LiteralPath $dataPath|ConvertFrom-Json
($data.data.data_slots|Where-Object id -eq 2).filename='chisburner87.ini'
($data.data.data_slots|Where-Object id -eq 3).filename='chisburner87.elf'
[IO.File]::WriteAllText($dataPath,($data|ConvertTo-Json -Depth 20),$utf8)
[IO.File]::WriteAllText((Join-Path $coreRoot 'info.txt'),
    'smoke.87: app-only 128-bank CRC32 mismatch locator on exact smoke.86 OS/RBF; read-only; GBA unchanged.',
    $utf8)

if((Sha (Join-Path $common 'os.bin')) -ne $baseOsHash -or
   (Sha (Join-Path $coreRoot 'chis.rbf_r')) -ne $baseRbfHash){
    throw 'Reused smoke.86 OS/RBF changed during packaging'
}

$readme=@"
ChisBurner - smoke.87 GB/GBC MBC5 逐 Bank CRC32 定位诊断

用途：smoke.86 已完成全部 2 MiB 读取，但 SHA-256 不等于参考文件。本版保持完全相同的
OS 和 FPGA RBF，只替换应用，按 128 个 16 KiB Bank 分别计算 CRC32，以定位差异来源。

参考 ROM：汉化版《梦见岛》，2097152 bytes，128 x 16 KiB banks。
SHA-256：8ba533be5908305a0f2f04117a858a91d4098ed8d4d2be6e48c3c3e9df1885e7
ROM 文件不包含在测试包中。

关机插入同一张 MBC5 烧录卡。把 SD_ROOT 合并到 SD 卡根目录并覆盖旧文件，然后启动：
  openFPGA -> ChisBurner -> Play Cartridge

请记录最终画面的：128 bank CRC32、BAD BANKS，以及失败时的 FIRST BAD BANK、
ACTUAL CRC、EXPECT CRC。

判读：
1. 128 bank CRC32 PASS / BAD BANKS 0：卡带读出的全部 2 MiB 与参考 ROM 一致，
   smoke.86 的差异来自目标端 SHA-256 实现。
2. BAD BANKS 非 0：卡带内容或对应 Bank 的读取与参考 ROM 不一致；首个 Bank 和 CRC
   可用于下一轮定点复读。

安全边界：本包精确复用 smoke.86 的只读 OS/RBF；应用不含任何写、擦除或烧录调用，
不访问 SRAM。GBA 源码与功能路径没有修改。
"@
[IO.File]::WriteAllText((Join-Path $bundle 'README_TEST87_CN.txt'),$readme,$utf8)

$provenance=@"
version=0.1.0-smoke.87-gb-mbc5-bank-crc-diagnostic
package_kind=APP_ONLY_DIAGNOSTIC
base_package=smoke.86-gb-mbc5-fullrom-sha256
base_os_exact_reuse=YES
base_rbf_exact_reuse=YES
os_sha256=$baseOsHash
rbf_r_sha256=$baseRbfHash
app_sha256=$(Sha $app)
reference_rom_bytes=2097152
reference_rom_sha256=8ba533be5908305a0f2f04117a858a91d4098ed8d4d2be6e48c3c3e9df1885e7
reference_rom_included=NO
expected_bank_count=128
expected_bank_bytes=16384
crc32_polynomial=0xEDB88320
save_access=ABSENT
nor_erase_program=ABSENT
gba_profile=UNCHANGED
"@
[IO.File]::WriteAllText((Join-Path $bundle 'BUILD_PROVENANCE.txt'),$provenance,$utf8)

Get-ChildItem -LiteralPath $sd -Filter '*.json' -Recurse -File|ForEach-Object{
    $null=Get-Content -Raw -LiteralPath $_.FullName|ConvertFrom-Json
}
if(Get-ChildItem -LiteralPath $sd -Recurse -File |
   Where-Object {$_.Extension -in @('.gb','.gbc','.gba')}){
    throw 'ROM image unexpectedly included in package'
}

$prefix=[IO.Path]::GetFullPath($sd).TrimEnd('\')+'\'
$sums=Get-ChildItem -LiteralPath $sd -Recurse -File|Sort-Object FullName|ForEach-Object{
    "$(Sha $_.FullName)  $($_.FullName.Substring($prefix.Length).Replace('\','/'))"
}
[IO.File]::WriteAllLines((Join-Path $bundle 'SHA256SUMS.txt'),[string[]]$sums,$utf8)
Compress-Archive -Path (Join-Path $bundle '*') -DestinationPath $zip -CompressionLevel Optimal
"MBC5 bank CRC package ready: $zip"
"ZIP SHA256: $(Sha $zip)"
