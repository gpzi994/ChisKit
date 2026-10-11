[CmdletBinding()]
param(
    [string]$Distro='Ubuntu-24.04',
    [string]$OpenFpgaSdk='/home/lin13/work/ONS_FPGA/openfpgaSDK',
    [string]$RomPath='C:/Users/lin13/Desktop/game/roms/mengjiandao.gbc',
    [string]$OsBuild='.integration/openfpgaos/src/firmware/os/bld/gb-header84-pocket-release',
    [string]$FpgaBuild='.integration/openfpgaos/src/fpga/targets/pocket/bld/chisburner-gb-header84-s1-win'
)

$ErrorActionPreference='Stop'
. (Join-Path $PSScriptRoot 'os-image-integrity.ps1')

function Sha([string]$Path) {
    (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant()
}
function Wsl([string]$Command) {
    & wsl.exe -d $Distro -- bash -lc $Command
    if($LASTEXITCODE -ne 0){throw "Build/test failed: $Command"}
}

$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$wroot='/mnt/'+$root.Substring(0,1).ToLowerInvariant()+'/'+$root.Substring(3).Replace('\','/')
$base=Join-Path $root 'build/smoke/ChisBurner-smoke.79b-playcart-runtime-rom-readonly/SD_ROOT'
$bundle=Join-Path $root 'build/smoke/ChisBurner-smoke.84-gb-mbc5-header-readonly'
$zip="$bundle.zip"
$rom=[IO.Path]::GetFullPath($RomPath)
$osRoot=if([IO.Path]::IsPathRooted($OsBuild)){$OsBuild}else{Join-Path $root $OsBuild}
$fpga=if([IO.Path]::IsPathRooted($FpgaBuild)){$FpgaBuild}else{Join-Path $root $FpgaBuild}
$os=Join-Path $osRoot 'os.bin'
$osManifest=Join-Path $osRoot 'GB_HEADER84_OS_SHA256.txt'
$rawRbf=Join-Path $fpga 'output_files/ap_core.rbf'
$fpgaManifest=Join-Path $fpga 'GB_HEADER84_SHA256.txt'
$qsf=Join-Path $fpga 'ap_core.qsf'
$sta=Join-Path $fpga 'output_files/ap_core.sta.rpt'
$app=Join-Path $root 'build/gb-mbc5-header84-app/app.elf'
$appRoot=Join-Path $root 'app/gb_mbc5_header84'
$utf8=[Text.UTF8Encoding]::new($false)

if((Test-Path -LiteralPath $bundle) -or (Test-Path -LiteralPath $zip)){
    throw 'Preserve existing smoke.84 package'
}
foreach($path in @($base,$rom,$os,$osManifest,$rawRbf,$fpgaManifest,$qsf,$sta,$appRoot)){
    if(!(Test-Path -LiteralPath $path)){throw "Missing package input: $path"}
}

if((Get-Item -LiteralPath $rom).Length -ne 2097152 -or
   (Sha $rom) -ne '8ba533be5908305a0f2f04117a858a91d4098ed8d4d2be6e48c3c3e9df1885e7'){
    throw 'Wrong MBC5 test ROM'
}
$romBytes=[IO.File]::ReadAllBytes($rom)
$headerCheck=0
for($i=0x134;$i -le 0x14c;$i++){$headerCheck=($headerCheck-$romBytes[$i]-1)-band 0xff}
if($romBytes[0x143] -ne 0x80 -or $romBytes[0x147] -ne 0x1b -or
   $romBytes[0x148] -ne 0x06 -or $romBytes[0x149] -ne 0x03 -or
   $romBytes[0x14d] -ne 0x3b -or $headerCheck -ne 0x3b -or
   $romBytes[0x14e] -ne 0x37 -or $romBytes[0x14f] -ne 0x28){
    throw 'MBC5 test ROM header contract changed'
}

foreach($manifest in @($osManifest,$fpgaManifest)){
    foreach($line in Get-Content -LiteralPath $manifest){
        if($line -match '^EXTRA_CFLAGS='){
            if($line -notmatch 'CART_MMIO_ENABLED=1' -or
               $line -notmatch 'CART_GB_HEADER_PROBE=1'){
                throw 'OS manifest is not the GB header-only policy'
            }
        } elseif($line -match '^([0-9a-f]{64})  (.+)$'){
            if(!(Test-Path -LiteralPath $Matches[2]) -or
               (Sha $Matches[2]) -cne $Matches[1]){
                throw "Build source drift: $line"
            }
        } else { throw "Malformed build manifest: $line" }
    }
}

$qsfText=Get-Content -Raw -LiteralPath $qsf
if($qsfText -notmatch '(?m)^set_global_assignment -name VERILOG_MACRO CHISBURNER_GB_HEADER84\s*$' -or
   $qsfText -match '(?m)^set_global_assignment -name VERILOG_MACRO CHISBURNER_GBA_PROGRAM65\s*$'){
    throw 'FPGA build is not isolated GB header84'
}
$staText=Get-Content -Raw -LiteralPath $sta
$slacks=@{}
foreach($kind in @('setup','hold')){
    $matches=[regex]::Matches($staText,"Worst-case $kind slack is\s+(-?\d+(?:\.\d+)?)")
    if(!$matches.Count){throw "Missing $kind timing result"}
    $value=($matches|ForEach-Object{[double]$_.Groups[1].Value}|Measure-Object -Minimum).Minimum
    if($value -le 0){throw "Refuse package: $kind slack $value ns"}
    $slacks[$kind]=$value
}
$null=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes($os))

$appSource=Get-Content -Raw -LiteralPath (Join-Path $appRoot 'main.c')
if($appSource -match 'of_cart_(write|erase|program)'){
    throw 'GB header app contains a cartridge mutation call'
}
Wsl "cd '$wroot/app/gb_mbc5_header84' && make -B all OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0"
if(!(Test-Path -LiteralPath $app)){throw 'GB header app build missing'}

if(-not ('ChisBurner.Gb84.Reverse' -as [type])){
    Add-Type -TypeDefinition @'
namespace ChisBurner.Gb84 {
    public static class Reverse {
        public static byte[] Apply(byte[] input) {
            var table = new byte[256];
            for (int i=0;i<256;i++) { int x=i,y=0; for(int b=0;b<8;b++){y=(y<<1)|(x&1);x>>=1;} table[i]=(byte)y; }
            var output=new byte[input.Length];
            for(int i=0;i<input.Length;i++) output[i]=table[input[i]];
            return output;
        }
    }
}
'@
}

$sd=Join-Path $bundle 'SD_ROOT'
New-Item -ItemType Directory -Path $bundle,$sd | Out-Null
$basePrefix=[IO.Path]::GetFullPath($base).TrimEnd('\')+'\'
foreach($file in Get-ChildItem -LiteralPath $base -Recurse -File){
    $rel=$file.FullName.Substring($basePrefix.Length).Replace('\','/')
    if($rel -in @('Assets/chisburner/common/chisburner79.elf',
                  'Assets/chisburner/common/chisburner79.ini',
                  'Assets/chisburner/common/db_AGB.json')){continue}
    $dest=Join-Path $sd $rel
    New-Item -ItemType Directory -Force (Split-Path $dest -Parent) | Out-Null
    Copy-Item -LiteralPath $file.FullName -Destination $dest
}

$common=Join-Path $sd 'Assets/chisburner/common'
$coreRoot=Join-Path $sd 'Cores/ChisBurner.ChisBurner'
Copy-Item -LiteralPath $os -Destination (Join-Path $common 'os.bin') -Force
Copy-Item -LiteralPath $app -Destination (Join-Path $common 'chisburner84.elf')
[IO.File]::WriteAllText((Join-Path $common 'chisburner84.ini'),
    "[os]`nELF=chisburner84.elf`nARGS=`n",$utf8)
[IO.File]::WriteAllBytes((Join-Path $coreRoot 'chis.rbf_r'),
    [ChisBurner.Gb84.Reverse]::Apply([IO.File]::ReadAllBytes($rawRbf)))

$corePath=Join-Path $coreRoot 'core.json'
$core=Get-Content -Raw -LiteralPath $corePath|ConvertFrom-Json
$core.core.metadata.description='GB/GBC MBC5 Bank 0 header read-only diagnostic'
$core.core.metadata.version='0.1.0-smoke.84-gb-mbc5-header-ro'
$core.core.metadata.date_release='2026-09-06'
if($core.core.framework.hardware.cartridge_adapter -ne 16777216){
    throw 'Package must retain Play Cartridge mode'
}
[IO.File]::WriteAllText($corePath,($core|ConvertTo-Json -Depth 20),$utf8)

$dataPath=Join-Path $coreRoot 'data.json'
$data=Get-Content -Raw -LiteralPath $dataPath|ConvertFrom-Json
($data.data.data_slots|Where-Object id -eq 2).filename='chisburner84.ini'
($data.data.data_slots|Where-Object id -eq 3).filename='chisburner84.elf'
$slot4=$data.data.data_slots|Where-Object id -eq 4
$slot4.name='Unused in smoke.84'
$slot4.extensions=@('gb','gbc')
$slot4.PSObject.Properties.Remove('size_exact')
[IO.File]::WriteAllText($dataPath,($data|ConvertTo-Json -Depth 20),$utf8)
[IO.File]::WriteAllText((Join-Path $coreRoot 'info.txt'),
    'smoke.84: GB/GBC MBC5 Bank 0 header read-only test; no mapper, RAM, erase, or program writes.', $utf8)

$readme=@"
ChisBurner - smoke.84 GB/GBC MBC5 Bank 0 只读预检

测试卡带：汉化版《梦见岛》MBC5 烧录卡。
ROM 参考文件不会复制进测试包；仅用以下已知值校验插入的实体卡带：
  大小 2097152 bytes，SHA-256 8ba533be5908305a0f2f04117a858a91d4098ed8d4d2be6e48c3c3e9df1885e7
  CGB=80，TYPE=1B (MBC5+RAM+BATTERY)，ROM=06 (2 MiB)，RAM=03 (32 KiB)
  Header checksum=3B；Global checksum 字段=3728，仅作信息显示；全 ROM 重算值=0028。

关机插入卡带。把 SD_ROOT 合并到 SD 卡根目录并确认覆盖旧文件，然后启动：
  openFPGA -> ChisBurner -> Play Cartridge

画面必须显示：GB/GBC MBC5 HEADER smoke.84
预期全部 PASS：
  service available / caps query / GB ROM-only caps / lifecycle ready
  safe idle / header pass 1 / header pass 2 / repeat identical
  Nintendo logo / header checksum / MBC5+RAM+BATTERY
  ROM 2 MiB / RAM 32K / CGB compatible / known header 3B / final safe idle
  GB HEADER PROBE: PASS

该版本只读取 Bank 0 的 0x0100-0x014F，共三段并重复一次。OS 与 FPGA 都拒绝
其他地址、长度与第 7 次读取。数据总线永久输入，/WR 永久非激活，顶层写使能恒为 0；
不会选择 MBC5 ROM bank，不会访问 SRAM，也不会执行擦除或烧写。

汉化 ROM 的全局校验字段 3728 与重新计算值 0028 不一致，因此 GLOBAL 仅作 INFO，不是失败项。
Quartus 全编译通过：minimum setup slack $($slacks.setup) ns，hold slack $($slacks.hold) ns。
"@
[IO.File]::WriteAllText((Join-Path $bundle 'README_TEST84_CN.txt'),$readme,$utf8)

$provenance=@"
version=0.1.0-smoke.84-gb-mbc5-header-ro
quartus_full_compile=PASS
minimum_setup_slack_ns=$($slacks.setup)
minimum_hold_slack_ns=$($slacks.hold)
reference_rom_bytes=2097152
reference_rom_sha256=8ba533be5908305a0f2f04117a858a91d4098ed8d4d2be6e48c3c3e9df1885e7
reference_rom_included=NO
rbf_sha256=$(Sha $rawRbf)
rbf_r_sha256=$(Sha (Join-Path $coreRoot 'chis.rbf_r'))
os_sha256=$(Sha (Join-Path $common 'os.bin'))
app_sha256=$(Sha (Join-Path $common 'chisburner84.elf'))
"@
[IO.File]::WriteAllText((Join-Path $bundle 'BUILD_PROVENANCE.txt'),$provenance,$utf8)

Get-ChildItem -LiteralPath $sd -Filter '*.json' -Recurse -File|ForEach-Object{
    $null=Get-Content -Raw -LiteralPath $_.FullName|ConvertFrom-Json
}
$prefix=[IO.Path]::GetFullPath($sd).TrimEnd('\')+'\'
$sums=Get-ChildItem -LiteralPath $sd -Recurse -File|Sort-Object FullName|ForEach-Object{
    "$(Sha $_.FullName)  $($_.FullName.Substring($prefix.Length).Replace('\','/'))"
}
[IO.File]::WriteAllLines((Join-Path $bundle 'SHA256SUMS.txt'),[string[]]$sums,$utf8)
Compress-Archive -Path (Join-Path $bundle '*') -DestinationPath $zip -CompressionLevel Optimal
"GB header84 package ready: $zip"
