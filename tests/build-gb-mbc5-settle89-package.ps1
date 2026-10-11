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
$bundle=Join-Path $root 'build/smoke/ChisBurner-smoke.89-gb-mbc5-initial-settle'
$zip="$bundle.zip"
$rom=[IO.Path]::GetFullPath($RomPath)
$appRoot=Join-Path $root 'app/gb_mbc5_settle89'
$locatorRoot=Join-Path $root 'app/gb_mbc5_bank0loc88'
$app=Join-Path $root 'build/gb-mbc5-settle89-app/app.elf'
$utf8=[Text.UTF8Encoding]::new($false)

if((Test-Path -LiteralPath $bundle) -or (Test-Path -LiteralPath $zip)){
    throw 'Preserve existing smoke.89 package'
}
foreach($path in @($baseBundle,$base,$rom,$appRoot,$locatorRoot)){
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
           (Get-Content -Raw -LiteralPath (Join-Path $locatorRoot 'main.c'))+
           (Get-Content -Raw -LiteralPath (Join-Path $locatorRoot 'chunk_crc32.c'))
if($appSource -match 'of_cart_(write|erase|program)'){
    throw 'Initial-settle app contains cartridge mutation call'
}
if($appSource -notmatch 'INITIAL_SETTLE_US 1000000u'){
    throw 'Required 1000 ms initial settle is absent'
}

Wsl "cd '$wroot' && cc -O2 -Wall -Wextra tests/test-bank0-chunkcrc88.c app/gb_mbc5_bank0loc88/chunk_crc32.c -o /tmp/test-bank0crc88 && /tmp/test-bank0crc88 '/mnt/c/Users/lin13/Desktop/game/roms/mengjiandao.gbc'"
Wsl "cd '$wroot/app/gb_mbc5_settle89' && make -B all OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0"
if(!(Test-Path -LiteralPath $app)){throw 'MBC5 initial-settle app build missing'}

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
Copy-Item -LiteralPath $app -Destination (Join-Path $common 'chisburner89.elf')
[IO.File]::WriteAllText((Join-Path $common 'chisburner89.ini'),"[os]`nELF=chisburner89.elf`nARGS=`n",$utf8)

$corePath=Join-Path $coreRoot 'core.json'
$core=Get-Content -Raw -LiteralPath $corePath|ConvertFrom-Json
$core.core.metadata.description='GB/GBC MBC5 1-second initial-settle read diagnostic'
$core.core.metadata.version='0.1.0-smoke.89-gb-mbc5-initial-settle'
$core.core.metadata.date_release='2026-09-06'
if($core.core.framework.hardware.cartridge_adapter -ne 16777216){throw 'Play Cartridge mode lost'}
[IO.File]::WriteAllText($corePath,($core|ConvertTo-Json -Depth 20),$utf8)

$dataPath=Join-Path $coreRoot 'data.json'
$data=Get-Content -Raw -LiteralPath $dataPath|ConvertFrom-Json
($data.data.data_slots|Where-Object id -eq 2).filename='chisburner89.ini'
($data.data.data_slots|Where-Object id -eq 3).filename='chisburner89.elf'
[IO.File]::WriteAllText($dataPath,($data|ConvertTo-Json -Depth 20),$utf8)
[IO.File]::WriteAllText((Join-Path $coreRoot 'info.txt'),
    'smoke.89: app-only 1000 ms pre-read settle plus Bank0 32-byte locator; exact smoke.86 OS/RBF; GBA unchanged.',
    $utf8)

if((Sha (Join-Path $common 'os.bin')) -ne $baseOsHash -or
   (Sha (Join-Path $coreRoot 'chis.rbf_r')) -ne $baseRbfHash){
    throw 'Reused smoke.86 OS/RBF changed during packaging'
}

$readme=@"
ChisBurner - smoke.89 GB/GBC MBC5 首次读取稳定性测试

smoke.88 已定位到只有第一次请求的 0x0000-0x001F 异常；后续全部数据一致，而且
smoke.87 与 smoke.88 的 Bank 0 实际 CRC 不同，符合首次读取尚未稳定的特征。

本版保持 smoke.88 的 32 字节定位逻辑，在 safe-idle 和 lifecycle ready 后等待整整
1000 ms，再发出第一个地址 0 的读取请求。OS 和 FPGA RBF 与 smoke.86 完全相同，
仍然严格顺序读取 2 MiB 一遍；没有额外读命令、SRAM 访问、NOR 擦除或烧写。

覆盖 SD_ROOT 后关机插入同一张卡，运行 Play Cartridge。请提供最终完整画面。
判定重点：bank0 CRC32、32-byte chunks、BAD CHUNKS。
若全部 PASS，后续正式 GB/GBC 核心将保留上电后首次访问的稳定等待；若仍只有地址
0000 失败，下一步改为 FPGA 内部丢弃一次预读，再返回正式数据。
"@
[IO.File]::WriteAllText((Join-Path $bundle 'README_TEST89_CN.txt'),$readme,$utf8)

$provenance=@"
version=0.1.0-smoke.89-gb-mbc5-initial-settle
package_kind=APP_ONLY_DIAGNOSTIC
base_package=smoke.86-gb-mbc5-fullrom-sha256
base_os_exact_reuse=YES
base_rbf_exact_reuse=YES
initial_settle_us=1000000
os_sha256=$baseOsHash
rbf_r_sha256=$baseRbfHash
app_sha256=$(Sha $app)
reference_rom_sha256=8ba533be5908305a0f2f04117a858a91d4098ed8d4d2be6e48c3c3e9df1885e7
reference_rom_included=NO
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
"MBC5 settle package ready: $zip"
"ZIP SHA256: $(Sha $zip)"
