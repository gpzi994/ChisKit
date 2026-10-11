[CmdletBinding()]
param(
    [string]$Distro='Ubuntu-24.04',
    [string]$OpenFpgaSdk='/home/lin13/work/ONS_FPGA/openfpgaSDK',
    [string]$RomPath='C:/Users/lin13/Desktop/game/roms/mengjiandao.gbc',
    [string]$OsBuild='.integration/openfpgaos/src/firmware/os/bld/gb-mbc5-fullrom86-pocket-release-r1',
    [string]$FpgaBuild='.integration/openfpgaos/src/fpga/targets/pocket/bld/chisburner-gb-mbc5-fullrom86-s2-win'
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
$base=Join-Path $root 'build/smoke/ChisBurner-smoke.85-gb-mbc5-bank-probe/SD_ROOT'
$bundle=Join-Path $root 'build/smoke/ChisBurner-smoke.86-gb-mbc5-fullrom-sha256'
$zip="$bundle.zip"
$rom=[IO.Path]::GetFullPath($RomPath)
$osRoot=if([IO.Path]::IsPathRooted($OsBuild)){$OsBuild}else{Join-Path $root $OsBuild}
$fpga=if([IO.Path]::IsPathRooted($FpgaBuild)){$FpgaBuild}else{Join-Path $root $FpgaBuild}
$os=Join-Path $osRoot 'os.bin'
$osManifest=Join-Path $osRoot 'GB_MBC5_FULLROM86_OS_SHA256.txt'
$rawRbf=Join-Path $fpga 'output_files/ap_core.rbf'
$fpgaManifest=Join-Path $fpga 'GB_MBC5_FULLROM86_SHA256.txt'
$qsf=Join-Path $fpga 'ap_core.qsf'
$sta=Join-Path $fpga 'output_files/ap_core.sta.rpt'
$app=Join-Path $root 'build/gb-mbc5-fullrom86-app/app.elf'
$appRoot=Join-Path $root 'app/gb_mbc5_fullrom86'
$rtl=Join-Path $root '.integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gb_mbc5_fullrom.v'
$utf8=[Text.UTF8Encoding]::new($false)

if((Test-Path -LiteralPath $bundle) -or (Test-Path -LiteralPath $zip)){
    throw 'Preserve existing smoke.86 package'
}
foreach($path in @($base,$rom,$os,$osManifest,$rawRbf,$fpgaManifest,$qsf,$sta,$appRoot,$rtl)){
    if(!(Test-Path -LiteralPath $path)){throw "Missing package input: $path"}
}
if((Get-Item -LiteralPath $rom).Length -ne 2097152 -or
   (Sha $rom) -ne '8ba533be5908305a0f2f04117a858a91d4098ed8d4d2be6e48c3c3e9df1885e7'){
    throw 'Wrong MBC5 test ROM'
}

foreach($manifest in @($osManifest,$fpgaManifest)){
    foreach($line in Get-Content -LiteralPath $manifest){
        if($line -match '^EXTRA_CFLAGS='){
            if($line -notmatch 'CART_MMIO_ENABLED=1' -or
               $line -notmatch 'CART_GB_MBC5_FULLROM_VERIFY=1'){
                throw 'OS manifest is not fullrom86 policy'
            }
        } elseif($line -match '^([0-9a-f]{64})  (.+)$'){
            if(!(Test-Path -LiteralPath $Matches[2]) -or (Sha $Matches[2]) -cne $Matches[1]){
                throw "Build source drift: $line"
            }
        } else { throw "Malformed build manifest: $line" }
    }
}

$qsfText=Get-Content -Raw -LiteralPath $qsf
if($qsfText -notmatch '(?m)^set_global_assignment -name VERILOG_MACRO CHISBURNER_GB_MBC5_FULLROM86\s*$' -or
   $qsfText -match '(?m)^set_global_assignment -name VERILOG_MACRO CHISBURNER_GBA_PROGRAM65\s*$'){
    throw 'FPGA build is not isolated MBC5 fullrom86'
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

$appSource=(Get-Content -Raw -LiteralPath (Join-Path $appRoot 'main.c'))+
           (Get-Content -Raw -LiteralPath (Join-Path $appRoot 'sha256.c'))
if($appSource -match 'of_cart_(write|erase|program)'){
    throw 'MBC5 full-ROM app contains cartridge mutation call'
}
$rtlSource=Get-Content -Raw -LiteralPath $rtl
if($rtlSource -notmatch 'gb_address\s*=\s*16''h2000' -or
   $rtlSource -match 'gb_address\s*=\s*16''h(0000|3000|4000|5555|2AAA)'){
    throw 'RTL write-address structure changed'
}
if($rtlSource -notmatch 'read_command_count < 17''d65536' -or
   $rtlSource -notmatch 'address_reg == expected_address'){
    throw 'Exact full-ROM ordering guard missing'
}

Wsl "cd '$wroot' && cc -O2 -Wall -Wextra tests/test-sha256-fullrom86.c app/gb_mbc5_fullrom86/sha256.c -o /tmp/test-sha86 && /tmp/test-sha86 '/mnt/c/Users/lin13/Desktop/game/roms/mengjiandao.gbc'"
Wsl "cd '$wroot' && rm -rf build/gb-fullrom86-package-rtl && verilator --cc --exe --build -j 4 -Wall -Wno-fatal --top-module chisburner_cart_gb_mbc5_fullrom -GADDRESS_SETUP_CYCLES=1 -GSELECT_SETUP_CYCLES=1 -GDATA_WAIT_CYCLES=1 -GRELEASE_CYCLES=1 -GWRITE_SETUP_CYCLES=1 -GWRITE_LOW_CYCLES=1 -GWRITE_RELEASE_CYCLES=1 -Mdir build/gb-fullrom86-package-rtl .integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gb_mbc5_fullrom.v '$wroot/tests/test-gb-mbc5-fullrom86-rtl.cpp' >/dev/null && build/gb-fullrom86-package-rtl/Vchisburner_cart_gb_mbc5_fullrom"
Wsl "cd '$wroot/app/gb_mbc5_fullrom86' && make -B all OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0"
if(!(Test-Path -LiteralPath $app)){throw 'MBC5 fullrom86 app build missing'}

if(-not ('ChisBurner.Gb86.Reverse' -as [type])){
    Add-Type -TypeDefinition @'
namespace ChisBurner.Gb86 {
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
    if($rel -in @('Assets/chisburner/common/chisburner85.elf','Assets/chisburner/common/chisburner85.ini')){continue}
    $dest=Join-Path $sd $rel
    New-Item -ItemType Directory -Force (Split-Path $dest -Parent) | Out-Null
    Copy-Item -LiteralPath $file.FullName -Destination $dest
}

$common=Join-Path $sd 'Assets/chisburner/common'
$coreRoot=Join-Path $sd 'Cores/ChisBurner.ChisBurner'
Copy-Item -LiteralPath $os -Destination (Join-Path $common 'os.bin') -Force
Copy-Item -LiteralPath $app -Destination (Join-Path $common 'chisburner86.elf')
[IO.File]::WriteAllText((Join-Path $common 'chisburner86.ini'),"[os]`nELF=chisburner86.elf`nARGS=`n",$utf8)
[IO.File]::WriteAllBytes((Join-Path $coreRoot 'chis.rbf_r'),
    [ChisBurner.Gb86.Reverse]::Apply([IO.File]::ReadAllBytes($rawRbf)))

$corePath=Join-Path $coreRoot 'core.json'
$core=Get-Content -Raw -LiteralPath $corePath|ConvertFrom-Json
$core.core.metadata.description='GB/GBC MBC5 exact 2 MiB full-ROM SHA-256 verifier'
$core.core.metadata.version='0.1.0-smoke.86-gb-mbc5-fullrom-sha256'
$core.core.metadata.date_release='2026-09-06'
if($core.core.framework.hardware.cartridge_adapter -ne 16777216){
    throw 'Package must retain Play Cartridge mode'
}
[IO.File]::WriteAllText($corePath,($core|ConvertTo-Json -Depth 20),$utf8)

$dataPath=Join-Path $coreRoot 'data.json'
$data=Get-Content -Raw -LiteralPath $dataPath|ConvertFrom-Json
($data.data.data_slots|Where-Object id -eq 2).filename='chisburner86.ini'
($data.data.data_slots|Where-Object id -eq 3).filename='chisburner86.elf'
[IO.File]::WriteAllText($dataPath,($data|ConvertTo-Json -Depth 20),$utf8)
[IO.File]::WriteAllText((Join-Path $coreRoot 'info.txt'),
    'smoke.86: exact 2 MiB MBC5 full-ROM read and SHA-256 verify; GBA profile unchanged; no SRAM/NOR erase/program.', $utf8)

$readme=@"
ChisBurner - smoke.86 GB/GBC MBC5 完整 2 MiB ROM 校验

测试卡带：已烧入汉化版《梦见岛》的 MBC5 烧录卡。
参考 ROM：2097152 bytes；128 x 16 KiB banks；SHA-256
8ba533be5908305a0f2f04117a858a91d4098ed8d4d2be6e48c3c3e9df1885e7
ROM 文件本身不会复制进测试包。

关机插入卡带。把 SD_ROOT 合并到 SD 卡根目录并确认覆盖旧文件，然后启动：
  openFPGA -> ChisBurner -> Play Cartridge

读取时画面会显示进度。完成后以下项目应全部 PASS：service/caps、lifecycle/start safe、
exact 2 MiB read、all 128 banks、CGB/MBC5 header、SHA-256 exact、final safe idle。
ACTUAL SHA-256 必须等于上述参考值，最终必须显示 FULL ROM VERIFY: PASS。

安全边界：OS 仅接受 0x000000-0x1FFFFF 内对齐的 32 字节 ROM 读取；FPGA 进一步强制
65536 个请求从 0 开始严格递增且只允许一遍。Bank 0 直接读取，Bank 1-127 的内部写脉冲
只允许访问 0x2000 低位 Bank 寄存器，并在每个块结束前恢复 Bank 1。不存在 0x0000、
0x3000、0x4000 写入，不访问 SRAM，不存在 NOR 解锁、擦除或烧写命令。

GB/GBC 功能通过独立编译宏增量加入；既有 GBA 读写引擎源码与功能路径未修改。
Quartus 全编译通过（seed 2）：minimum setup slack $($slacks.setup) ns，hold slack $($slacks.hold) ns。
"@
[IO.File]::WriteAllText((Join-Path $bundle 'README_TEST86_CN.txt'),$readme,$utf8)

$gbaSources=@(
    (Join-Path $root '.integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gba_readonly.v'),
    (Join-Path $root '.integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gba_s29gl256.v')
)
$provenance=@"
version=0.1.0-smoke.86-gb-mbc5-fullrom-sha256
quartus_full_compile=PASS
quartus_seed=2
minimum_setup_slack_ns=$($slacks.setup)
minimum_hold_slack_ns=$($slacks.hold)
reference_rom_bytes=2097152
reference_rom_sha256=8ba533be5908305a0f2f04117a858a91d4098ed8d4d2be6e48c3c3e9df1885e7
reference_rom_included=NO
read_commands=65536_EXACT_ASCENDING
mapper_write_address=0x2000_ONLY
mapper_high_bank_write=ABSENT
save_access=ABSENT
nor_erase_program=ABSENT
gba_profile=UNCHANGED_NOT_INSTANTIATED_IN_THIS_BUILD
gba_readonly_source_sha256=$(Sha $gbaSources[0])
gba_programmer_source_sha256=$(Sha $gbaSources[1])
rbf_sha256=$(Sha $rawRbf)
rbf_r_sha256=$(Sha (Join-Path $coreRoot 'chis.rbf_r'))
os_sha256=$(Sha (Join-Path $common 'os.bin'))
app_sha256=$(Sha (Join-Path $common 'chisburner86.elf'))
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
"MBC5 fullrom86 package ready: $zip"
"ZIP SHA256: $(Sha $zip)"
