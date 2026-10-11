[CmdletBinding()]
param(
    [string]$Distro='Ubuntu-24.04',
    [string]$OpenFpgaSdk='/home/lin13/work/ONS_FPGA/openfpgaSDK',
    [string]$RomPath='C:/Users/lin13/Desktop/game/roms/mengjiandao.gbc'
)

$ErrorActionPreference='Stop'
. (Join-Path $PSScriptRoot 'os-image-integrity.ps1')
function Sha([string]$Path){(Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant()}
function Wsl([string]$Command){
    & wsl.exe -d $Distro -- bash -lc $Command
    if($LASTEXITCODE -ne 0){throw "Build/test failed: $Command"}
}

$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$wroot='/mnt/'+$root.Substring(0,1).ToLowerInvariant()+'/'+$root.Substring(3).Replace('\','/')
$baseBundle=Join-Path $root 'build/smoke/ChisBurner-smoke.86-gb-mbc5-fullrom-sha256'
$base=Join-Path $baseBundle 'SD_ROOT'
$bundle=Join-Path $root 'build/smoke/ChisBurner-smoke.88-gb-mbc5-bank0-32byte-locator'
$zip="$bundle.zip"
$rom=[IO.Path]::GetFullPath($RomPath)
$appRoot=Join-Path $root 'app/gb_mbc5_bank0loc88'
$app=Join-Path $root 'build/gb-mbc5-bank0loc88-app/app.elf'
$utf8=[Text.UTF8Encoding]::new($false)

if((Test-Path -LiteralPath $bundle) -or (Test-Path -LiteralPath $zip)){
    throw 'Preserve existing smoke.88 package'
}
foreach($path in @($baseBundle,$base,$rom,$appRoot)){
    if(!(Test-Path -LiteralPath $path)){throw "Missing package input: $path"}
}
if((Get-Item -LiteralPath $rom).Length -ne 2097152 -or
   (Sha $rom) -ne '8ba533be5908305a0f2f04117a858a91d4098ed8d4d2be6e48c3c3e9df1885e7'){
    throw 'Wrong MBC5 reference ROM'
}

foreach($line in Get-Content -LiteralPath (Join-Path $baseBundle 'SHA256SUMS.txt')){
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
           (Get-Content -Raw -LiteralPath (Join-Path $appRoot 'chunk_crc32.c'))
if($appSource -match 'of_cart_(write|erase|program)'){
    throw 'Bank0 locator app contains cartridge mutation call'
}

Wsl "cd '$wroot' && cc -O2 -Wall -Wextra tests/test-bank0-chunkcrc88.c app/gb_mbc5_bank0loc88/chunk_crc32.c -o /tmp/test-bank0crc88 && /tmp/test-bank0crc88 '/mnt/c/Users/lin13/Desktop/game/roms/mengjiandao.gbc'"
Wsl "cd '$wroot/app/gb_mbc5_bank0loc88' && make -B all OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0"
if(!(Test-Path -LiteralPath $app)){throw 'MBC5 bank0 locator app build missing'}

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
Copy-Item -LiteralPath $app -Destination (Join-Path $common 'chisburner88.elf')
[IO.File]::WriteAllText((Join-Path $common 'chisburner88.ini'),"[os]`nELF=chisburner88.elf`nARGS=`n",$utf8)

$corePath=Join-Path $coreRoot 'core.json'
$core=Get-Content -Raw -LiteralPath $corePath|ConvertFrom-Json
$core.core.metadata.description='GB/GBC MBC5 Bank 0 32-byte mismatch locator'
$core.core.metadata.version='0.1.0-smoke.88-gb-mbc5-bank0-32byte-locator'
$core.core.metadata.date_release='2026-09-06'
if($core.core.framework.hardware.cartridge_adapter -ne 16777216){throw 'Play Cartridge mode lost'}
[IO.File]::WriteAllText($corePath,($core|ConvertTo-Json -Depth 20),$utf8)

$dataPath=Join-Path $coreRoot 'data.json'
$data=Get-Content -Raw -LiteralPath $dataPath|ConvertFrom-Json
($data.data.data_slots|Where-Object id -eq 2).filename='chisburner88.ini'
($data.data.data_slots|Where-Object id -eq 3).filename='chisburner88.elf'
[IO.File]::WriteAllText($dataPath,($data|ConvertTo-Json -Depth 20),$utf8)
[IO.File]::WriteAllText((Join-Path $coreRoot 'info.txt'),
    'smoke.88: app-only Bank0 512x32-byte CRC locator on exact smoke.86 OS/RBF; read-only; GBA unchanged.',
    $utf8)

if((Sha (Join-Path $common 'os.bin')) -ne $baseOsHash -or
   (Sha (Join-Path $coreRoot 'chis.rbf_r')) -ne $baseRbfHash){
    throw 'Reused smoke.86 OS/RBF changed during packaging'
}

$readme=@"
ChisBurner - smoke.88 GB/GBC MBC5 Bank 0 精确定位

smoke.87 结果：仅 Bank 0 失败，实际 CRC32 39a766d6，参考 CRC32 d0a2a03f；
其余 127 个 Bank 全部一致。本版把 Bank 0 的 0x0000-0x3FFF 切为 512 个 32 字节块，
逐块与参考 ROM 的 CRC32 对比，并显示首末异常地址及首个异常块的实际 32 字节。

本版是应用层诊断，精确复用 smoke.86 已实机通过的 OS 和 FPGA RBF。仍会按硬件约束
顺序读取完整 2 MiB 一遍，不包含任何 SRAM、NOR 擦除或烧写调用，不修改 GBA 路径。

把 SD_ROOT 合并到 SD 卡根目录并覆盖旧文件，关机插入同一张卡后启动 Play Cartridge。
请拍摄最终完整画面，重点记录：BAD CHUNKS、FIRST、LAST、CRC ACT/REF，以及
FIRST BAD BLOCK DATA 的四行字节。
"@
[IO.File]::WriteAllText((Join-Path $bundle 'README_TEST88_CN.txt'),$readme,$utf8)

$provenance=@"
version=0.1.0-smoke.88-gb-mbc5-bank0-32byte-locator
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
bank0_chunk_count=512
bank0_chunk_bytes=32
crc32_polynomial=0xEDB88320
save_access=ABSENT
nor_erase_program=ABSENT
gba_profile=UNCHANGED
"@
[IO.File]::WriteAllText((Join-Path $bundle 'BUILD_PROVENANCE.txt'),$provenance,$utf8)

Get-ChildItem -LiteralPath $sd -Filter '*.json' -Recurse -File|ForEach-Object{
    $null=Get-Content -Raw -LiteralPath $_.FullName|ConvertFrom-Json
}
if(Get-ChildItem -LiteralPath $sd -Recurse -File|Where-Object {$_.Extension -in @('.gb','.gbc','.gba')}){
    throw 'ROM image unexpectedly included in package'
}
$prefix=[IO.Path]::GetFullPath($sd).TrimEnd('\')+'\'
$sums=Get-ChildItem -LiteralPath $sd -Recurse -File|Sort-Object FullName|ForEach-Object{
    "$(Sha $_.FullName)  $($_.FullName.Substring($prefix.Length).Replace('\','/'))"
}
[IO.File]::WriteAllLines((Join-Path $bundle 'SHA256SUMS.txt'),[string[]]$sums,$utf8)
Compress-Archive -Path (Join-Path $bundle '*') -DestinationPath $zip -CompressionLevel Optimal
"MBC5 bank0 locator package ready: $zip"
"ZIP SHA256: $(Sha $zip)"
