[CmdletBinding()]
param(
    [string]$Distro='Ubuntu-24.04',
    [string]$OpenFpgaSdk='/home/lin13/work/ONS_FPGA/openfpgaSDK',
    [string]$RomPath='C:/Users/lin13/Desktop/game/roms/mengjiandao.gbc',
    [string]$OsBuild='.integration/openfpgaos/src/firmware/os/bld/gb-mbc5-banks85-pocket-release',
    [string]$FpgaBuild='.integration/openfpgaos/src/fpga/targets/pocket/bld/chisburner-gb-mbc5-banks85-s2-win'
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
$base=Join-Path $root 'build/smoke/ChisBurner-smoke.84-gb-mbc5-header-readonly/SD_ROOT'
$bundle=Join-Path $root 'build/smoke/ChisBurner-smoke.85-gb-mbc5-bank-probe'
$zip="$bundle.zip"
$rom=[IO.Path]::GetFullPath($RomPath)
$osRoot=if([IO.Path]::IsPathRooted($OsBuild)){$OsBuild}else{Join-Path $root $OsBuild}
$fpga=if([IO.Path]::IsPathRooted($FpgaBuild)){$FpgaBuild}else{Join-Path $root $FpgaBuild}
$os=Join-Path $osRoot 'os.bin'
$osManifest=Join-Path $osRoot 'GB_MBC5_BANKS85_OS_SHA256.txt'
$rawRbf=Join-Path $fpga 'output_files/ap_core.rbf'
$fpgaManifest=Join-Path $fpga 'GB_MBC5_BANKS85_SHA256.txt'
$qsf=Join-Path $fpga 'ap_core.qsf'
$sta=Join-Path $fpga 'output_files/ap_core.sta.rpt'
$app=Join-Path $root 'build/gb-mbc5-banks85-app/app.elf'
$appRoot=Join-Path $root 'app/gb_mbc5_banks85'
$rtl=Join-Path $root '.integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gb_mbc5_probe.v'
$utf8=[Text.UTF8Encoding]::new($false)

if((Test-Path -LiteralPath $bundle) -or (Test-Path -LiteralPath $zip)){
    throw 'Preserve existing smoke.85 package'
}
foreach($path in @($base,$rom,$os,$osManifest,$rawRbf,$fpgaManifest,$qsf,$sta,$appRoot,$rtl)){
    if(!(Test-Path -LiteralPath $path)){throw "Missing package input: $path"}
}
if((Get-Item -LiteralPath $rom).Length -ne 2097152 -or
   (Sha $rom) -ne '8ba533be5908305a0f2f04117a858a91d4098ed8d4d2be6e48c3c3e9df1885e7'){
    throw 'Wrong MBC5 test ROM'
}

$romBytes=[IO.File]::ReadAllBytes($rom)
$addresses=@(0x4000,0x8000,0x7c000,0x80000,0xfc000,0x100000,0x1fc000,0x1fffe0)
$expected=@(
 'FA96DBC71240424073407F409C40AA402558CDD644F0FEA728282110DC0E80F3',
 '0606060606060606060606060606060606060606060606060606060606060606',
 'C30940C35C7BC31E402100D336002C20FB3E80E0263E77E0243EFFE025C9CD04',
 '346A03614419966603E37B18B26903285303495203DD7B07667918E95703266A',
 '7C5FFA80CCFE012824FA95DBFE013E7EC215403E7FEAE8FFFA64C1A7FA70C120',
 'FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF005801420017002101AB01AC003703',
 '0000000000000000000000000000000000000000000000000000000000000000',
 '770000000000007C7B7A5E2D5E2D623562355E2D5E2D623562356235623562FF'
)
for($i=0;$i-lt $addresses.Count;$i++){
    $actual=($romBytes[$addresses[$i]..($addresses[$i]+31)]|ForEach-Object{$_.ToString('X2')})-join''
    if($actual -cne $expected[$i]){throw "Reference window changed at 0x$($addresses[$i].ToString('X'))"}
}

foreach($manifest in @($osManifest,$fpgaManifest)){
    foreach($line in Get-Content -LiteralPath $manifest){
        if($line -match '^EXTRA_CFLAGS='){
            if($line -notmatch 'CART_MMIO_ENABLED=1' -or
               $line -notmatch 'CART_GB_MBC5_BANK_PROBE=1'){
                throw 'OS manifest is not MBC5 banks85 policy'
            }
        } elseif($line -match '^([0-9a-f]{64})  (.+)$'){
            if(!(Test-Path -LiteralPath $Matches[2]) -or (Sha $Matches[2]) -cne $Matches[1]){
                throw "Build source drift: $line"
            }
        } else { throw "Malformed build manifest: $line" }
    }
}

$qsfText=Get-Content -Raw -LiteralPath $qsf
if($qsfText -notmatch '(?m)^set_global_assignment -name VERILOG_MACRO CHISBURNER_GB_MBC5_BANK85\s*$' -or
   $qsfText -match '(?m)^set_global_assignment -name VERILOG_MACRO CHISBURNER_GBA_PROGRAM65\s*$'){
    throw 'FPGA build is not isolated MBC5 banks85'
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
    throw 'MBC5 bank app contains cartridge mutation call'
}
$rtlSource=Get-Content -Raw -LiteralPath $rtl
if($rtlSource -notmatch 'gb_address\s*=\s*16''h2000' -or
   $rtlSource -match 'gb_address\s*=\s*16''h(0000|3000|4000|5555|2AAA)'){
    throw 'RTL write-address structure changed'
}

Wsl "cd '$wroot' && rm -rf build/gb-banks85-package-rtl && verilator --cc --exe --build -j 4 -Wall -Wno-fatal --top-module chisburner_cart_gb_mbc5_probe -Mdir build/gb-banks85-package-rtl .integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gb_mbc5_probe.v '$wroot/tests/test-gb-mbc5-banks85-rtl.cpp' >/dev/null && build/gb-banks85-package-rtl/Vchisburner_cart_gb_mbc5_probe"
Wsl "cd '$wroot/app/gb_mbc5_banks85' && make -B all OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0"
if(!(Test-Path -LiteralPath $app)){throw 'MBC5 banks85 app build missing'}

if(-not ('ChisBurner.Gb85.Reverse' -as [type])){
    Add-Type -TypeDefinition @'
namespace ChisBurner.Gb85 {
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
    if($rel -in @('Assets/chisburner/common/chisburner84.elf','Assets/chisburner/common/chisburner84.ini')){continue}
    $dest=Join-Path $sd $rel
    New-Item -ItemType Directory -Force (Split-Path $dest -Parent) | Out-Null
    Copy-Item -LiteralPath $file.FullName -Destination $dest
}

$common=Join-Path $sd 'Assets/chisburner/common'
$coreRoot=Join-Path $sd 'Cores/ChisBurner.ChisBurner'
Copy-Item -LiteralPath $os -Destination (Join-Path $common 'os.bin') -Force
Copy-Item -LiteralPath $app -Destination (Join-Path $common 'chisburner85.elf')
[IO.File]::WriteAllText((Join-Path $common 'chisburner85.ini'),"[os]`nELF=chisburner85.elf`nARGS=`n",$utf8)
[IO.File]::WriteAllBytes((Join-Path $coreRoot 'chis.rbf_r'),
    [ChisBurner.Gb85.Reverse]::Apply([IO.File]::ReadAllBytes($rawRbf)))

$corePath=Join-Path $coreRoot 'core.json'
$core=Get-Content -Raw -LiteralPath $corePath|ConvertFrom-Json
$core.core.metadata.description='GB/GBC MBC5 constrained ROM bank paging diagnostic'
$core.core.metadata.version='0.1.0-smoke.85-gb-mbc5-bank-probe'
$core.core.metadata.date_release='2026-09-06'
if($core.core.framework.hardware.cartridge_adapter -ne 16777216){
    throw 'Package must retain Play Cartridge mode'
}
[IO.File]::WriteAllText($corePath,($core|ConvertTo-Json -Depth 20),$utf8)

$dataPath=Join-Path $coreRoot 'data.json'
$data=Get-Content -Raw -LiteralPath $dataPath|ConvertFrom-Json
($data.data.data_slots|Where-Object id -eq 2).filename='chisburner85.ini'
($data.data.data_slots|Where-Object id -eq 3).filename='chisburner85.elf'
[IO.File]::WriteAllText($dataPath,($data|ConvertTo-Json -Depth 20),$utf8)
[IO.File]::WriteAllText((Join-Path $coreRoot 'info.txt'),
    'smoke.85: constrained MBC5 ROM-bank select/read/restore; no SRAM, NOR erase, or NOR program commands.', $utf8)

$readme=@"
ChisBurner - smoke.85 GB/GBC MBC5 固定分页读取测试

测试卡带：已烧入汉化版《梦见岛》的 MBC5 烧录卡。
参考 ROM：2097152 bytes；SHA-256
8ba533be5908305a0f2f04117a858a91d4098ed8d4d2be6e48c3c3e9df1885e7
ROM 文件本身不会复制进测试包。

关机插入卡带。把 SD_ROOT 合并到 SD 卡根目录并确认覆盖旧文件，然后启动：
  openFPGA -> ChisBurner -> Play Cartridge

画面必须显示：GB/GBC MBC5 BANKS smoke.85
以下项目应全部 PASS：service/caps/lifecycle/safe idle，Bank 1、2、31、32、63、64、
127 起始窗口，Bank 127 最后 32 字节，以及 expected bytes、repeat identical、final safe idle。
最终必须显示：MBC5 PAGING: PASS。

安全边界：OS 只接受上述 8 个地址、固定 32 字节长度；FPGA 还强制按该顺序读取两遍，
第 17 次读取被拒绝。每次读取只允许在 0x2000 写入目标 Bank 低 8 位，读取完成后只在
0x2000 写 01 恢复 Bank 1。因 ROM 仅 2 MiB，绝不写 0x3000；不写 0x0000/0x4000，
不访问 SRAM，也不存在 NOR 解锁、擦除或烧写命令。

Quartus 全编译通过（seed 2）：minimum setup slack $($slacks.setup) ns，hold slack $($slacks.hold) ns。
"@
[IO.File]::WriteAllText((Join-Path $bundle 'README_TEST85_CN.txt'),$readme,$utf8)

$provenance=@"
version=0.1.0-smoke.85-gb-mbc5-bank-probe
quartus_full_compile=PASS
quartus_seed=2
minimum_setup_slack_ns=$($slacks.setup)
minimum_hold_slack_ns=$($slacks.hold)
reference_rom_bytes=2097152
reference_rom_sha256=8ba533be5908305a0f2f04117a858a91d4098ed8d4d2be6e48c3c3e9df1885e7
reference_rom_included=NO
mapper_write_address=0x2000_ONLY
mapper_high_bank_write=ABSENT
save_access=ABSENT
nor_erase_program=ABSENT
rbf_sha256=$(Sha $rawRbf)
rbf_r_sha256=$(Sha (Join-Path $coreRoot 'chis.rbf_r'))
os_sha256=$(Sha (Join-Path $common 'os.bin'))
app_sha256=$(Sha (Join-Path $common 'chisburner85.elf'))
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
"MBC5 banks85 package ready: $zip"
