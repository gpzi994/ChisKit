[CmdletBinding()]
param(
    [string]$Distro='Ubuntu-24.04',
    [string]$OpenFpgaSdk='/home/lin13/work/ONS_FPGA/openfpgaSDK',
    [string]$OsBuild='.integration/openfpgaos/src/firmware/os/bld/gb-mbc5-ident91-pocket-release',
    [string]$FpgaBuild='.integration/openfpgaos/src/fpga/targets/pocket/bld/chisburner-gb-mbc5-ident91-s2-win'
)
$ErrorActionPreference='Stop'
. (Join-Path $PSScriptRoot 'os-image-integrity.ps1')
function Sha([string]$Path){(Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant()}
function Wsl([string]$Command){
    & wsl.exe -d $Distro -- bash -lc $Command
    if($LASTEXITCODE-ne 0){throw "Build/test failed: $Command"}
}
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$wroot='/mnt/'+$root.Substring(0,1).ToLowerInvariant()+'/'+$root.Substring(3).Replace('\','/')
$base=Join-Path $root 'build/smoke/ChisBurner-smoke.90-gb-mbc5-first-read-discard/SD_ROOT'
$bundle=Join-Path $root 'build/smoke/ChisBurner-smoke.91-gb-mbc5-nor-identify-r1'
$zip="$bundle.zip"
$osRoot=if([IO.Path]::IsPathRooted($OsBuild)){$OsBuild}else{Join-Path $root $OsBuild}
$fpga=if([IO.Path]::IsPathRooted($FpgaBuild)){$FpgaBuild}else{Join-Path $root $FpgaBuild}
$os=Join-Path $osRoot 'os.bin'; $osManifest=Join-Path $osRoot 'GB_MBC5_IDENT91_OS_SHA256.txt'
$rawRbf=Join-Path $fpga 'output_files/ap_core.rbf'; $qsf=Join-Path $fpga 'ap_core.qsf'
$fpgaManifest=Join-Path $fpga 'GB_MBC5_IDENT91_SHA256.txt'
$sta=Join-Path $fpga 'output_files/ap_core.sta.rpt'
$appRoot=Join-Path $root 'app/gb_mbc5_ident91'; $app=Join-Path $root 'build/gb-mbc5-ident91-app/app.elf'
$rtl=Join-Path $root '.integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gb_mbc5_identify.v'
$gbaRead=Join-Path $root '.integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gba_readonly.v'
$gbaProgram=Join-Path $root '.integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gba_s29gl256.v'
$utf8=[Text.UTF8Encoding]::new($false)
if((Test-Path $bundle)-or(Test-Path $zip)){throw 'Preserve existing smoke.91 package'}
foreach($p in @($base,$os,$osManifest,$rawRbf,$qsf,$fpgaManifest,$sta,$app,$appRoot,$rtl,$gbaRead,$gbaProgram)){
    if(!(Test-Path $p)){throw "Missing package input: $p"}
}
foreach($manifest in @($osManifest,$fpgaManifest)){
    foreach($line in Get-Content $manifest){
        if($line -match '^EXTRA_CFLAGS='){
            if($line -notmatch 'CART_MMIO_ENABLED=1' -or $line -notmatch 'CART_GB_MBC5_IDENTIFY=1'){
                throw 'OS manifest is not identify91 policy'
            }
        } elseif($line -match '^([0-9a-f]{64})  (.+)$'){
            if(!(Test-Path $Matches[2])-or(Sha $Matches[2])-cne $Matches[1]){throw "Source drift: $line"}
        } else {throw "Malformed manifest: $line"}
    }
}
$qsfText=Get-Content -Raw $qsf
if($qsfText-notmatch'(?m)^set_global_assignment -name VERILOG_MACRO CHISBURNER_GB_MBC5_IDENT91\s*$'-or
   $qsfText-match'(?m)^set_global_assignment -name VERILOG_MACRO CHISBURNER_GBA_PROGRAM65\s*$'){
    throw 'FPGA build is not isolated identify91'
}
$staText=Get-Content -Raw $sta; $slacks=@{}
foreach($kind in @('setup','hold')){
    $m=[regex]::Matches($staText,"Worst-case $kind slack is\s+(-?\d+(?:\.\d+)?)")
    if(!$m.Count){throw "Missing $kind timing"}
    $v=($m|ForEach-Object{[double]$_.Groups[1].Value}|Measure-Object -Minimum).Minimum
    if($v-le 0){throw "Timing failed: $kind $v ns"}; $slacks[$kind]=$v
}
$null=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes($os))
$appSource=Get-Content -Raw (Join-Path $appRoot 'main.c')
if($appSource-match'of_cart_(write|erase|program)' -or $appSource-notmatch'OF_CART_SPACE_IDENTIFY'){
    throw 'App safety contract changed'
}
$rtlSource=Get-Content -Raw $rtl
foreach($required in @("16'h0AAA","16'h0555","16'h00AA","8'hAA","8'h55","8'h90","8'h98","8'hF0","8'hFF")){
    if($rtlSource-notmatch[regex]::Escape($required)){throw "Fixed identify token missing: $required"}
}
if($rtlSource-match'CMD_ERASE|CMD_PROGRAM|8''h80|8''hA0|8''h25|8''h29|8''h30'){
    throw 'Destructive command token found in identify RTL'
}
Wsl "cd '$wroot' && rm -rf build/gb-ident91-package-rtl && verilator --cc --exe --build -j 4 -Wall -Wno-fatal --top-module chisburner_cart_gb_mbc5_identify -GADDRESS_SETUP_CYCLES=1 -GSELECT_SETUP_CYCLES=1 -GDATA_WAIT_CYCLES=1 -GRELEASE_CYCLES=1 -GWRITE_SETUP_CYCLES=1 -GWRITE_LOW_CYCLES=1 -GWRITE_RELEASE_CYCLES=1 -Mdir build/gb-ident91-package-rtl .integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gb_mbc5_identify.v '$wroot/tests/test-gb-mbc5-ident91-rtl.cpp' >/dev/null && build/gb-ident91-package-rtl/Vchisburner_cart_gb_mbc5_identify"
Wsl "cd '$wroot' && rm -rf build/gba-ident91-package-rtl && verilator --cc --exe --build -j 4 -Wall -Wno-fatal --top-module chisburner_cart_gba_s29gl256 -Mdir build/gba-ident91-package-rtl .integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gba_readonly.v .integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gba_s29gl256.v '$wroot/tests/test-program65-rtl.cpp' >/dev/null && build/gba-ident91-package-rtl/Vchisburner_cart_gba_s29gl256"
Wsl "cd '$wroot/app/gb_mbc5_ident91' && make -B all OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0"
if(-not('ChisBurner.Gb91.Reverse'-as[type])){Add-Type -TypeDefinition @'
namespace ChisBurner.Gb91 { public static class Reverse { public static byte[] Apply(byte[] i) { var t=new byte[256]; for(int n=0;n<256;n++){int x=n,y=0;for(int b=0;b<8;b++){y=(y<<1)|(x&1);x>>=1;}t[n]=(byte)y;}var o=new byte[i.Length];for(int n=0;n<i.Length;n++)o[n]=t[i[n]];return o; } } }
'@}
$sd=Join-Path $bundle 'SD_ROOT'; New-Item -ItemType Directory -Path $bundle,$sd|Out-Null
$prefix=[IO.Path]::GetFullPath($base).TrimEnd('\')+'\'
foreach($file in Get-ChildItem $base -Recurse -File){
    $rel=$file.FullName.Substring($prefix.Length).Replace('\','/')
    if($rel-in@('Assets/chisburner/common/chisburner90.elf','Assets/chisburner/common/chisburner90.ini','Cores/ChisBurner.ChisBurner/chis.rbf_r')){continue}
    $dest=Join-Path $sd $rel; New-Item -ItemType Directory -Force (Split-Path $dest -Parent)|Out-Null
    Copy-Item $file.FullName $dest
}
$common=Join-Path $sd 'Assets/chisburner/common'; $coreRoot=Join-Path $sd 'Cores/ChisBurner.ChisBurner'
Copy-Item $os (Join-Path $common 'os.bin') -Force; Copy-Item $app (Join-Path $common 'chisburner91.elf')
[IO.File]::WriteAllText((Join-Path $common 'chisburner91.ini'),"[os]`nELF=chisburner91.elf`nARGS=`n",$utf8)
[IO.File]::WriteAllBytes((Join-Path $coreRoot 'chis.rbf_r'),[ChisBurner.Gb91.Reverse]::Apply([IO.File]::ReadAllBytes($rawRbf)))
$corePath=Join-Path $coreRoot 'core.json'; $core=Get-Content -Raw $corePath|ConvertFrom-Json
$core.core.metadata.description='GB/GBC MBC5 fixed-command JEDEC/CFI NOR identifier'
$core.core.metadata.version='0.1.0-smoke.91-gb-mbc5-nor-identify'; $core.core.metadata.date_release='2026-09-07'
if($core.core.framework.hardware.cartridge_adapter-ne 16777216){throw 'Play Cartridge mode lost'}
[IO.File]::WriteAllText($corePath,($core|ConvertTo-Json -Depth 20),$utf8)
$dataPath=Join-Path $coreRoot 'data.json'; $data=Get-Content -Raw $dataPath|ConvertFrom-Json
($data.data.data_slots|Where-Object {$_.id -eq 2}).filename='chisburner91.ini'
($data.data.data_slots|Where-Object {$_.id -eq 3}).filename='chisburner91.elf'
[IO.File]::WriteAllText($dataPath,($data|ConvertTo-Json -Depth 20),$utf8)
[IO.File]::WriteAllText((Join-Path $coreRoot 'info.txt'),'smoke.91: fixed Mori-compatible MBC5 JEDEC/CFI identification only; no ROM/SRAM/erase/program; GBA unchanged.',$utf8)
$readme=@"
ChisBurner - smoke.91 GB/GBC MBC5 NOR 固定识别测试

本阶段只识别烧录卡 NOR，不擦除、不烧写、不访问 SRAM，也不开放普通 ROM 读取。
FPGA 内部固定执行 Mori 标准 MBC5 路径：丢弃首次物理读取；F0 复位；读取正常模式
基线；0AAA=AA、0555=55、0AAA=90 后读取 JEDEC；F0 退出；先试 00AA=98，
若 Q/R/Y 不成立则 F0 后再试 0000=98；最后使用对应 F0/FF 退出并读取 0000
确认恢复正常模式。应用无法提交任何写地址或数据，擦除/烧写操作码不存在。

安装：关机，把 SD_ROOT 合并至 SD 卡根目录并确认覆盖旧文件；插入同一张 MBC5
烧录卡，启动 Play Cartridge。第 1 页 CAPS 与 SAFE START 应 PASS。按 A 一次，完成后
拍摄第 3 页和第 4 页。关键结果：IDENTIFY RC 0、ID CHANGED PASS、QRY PASS、
FINAL NORMAL PASS、SAFE PASS。请保留两页全部十六进制字节，这些数据用于确定下一阶段
具体 NOR profile。即使 QRY FAIL，也不要重复按键或更换版本，直接发送两页照片。

GBA 引擎未改动；program81 完整 RTL 回归已通过。Quartus seed 2 全编译通过：
setup $($slacks.setup) ns，hold $($slacks.hold) ns。
"@
[IO.File]::WriteAllText((Join-Path $bundle 'README_TEST91_CN.txt'),$readme,$utf8)
$prov=@"
version=0.1.0-smoke.91-gb-mbc5-nor-identify
quartus_full_compile=PASS
quartus_seed=2
minimum_setup_slack_ns=$($slacks.setup)
minimum_hold_slack_ns=$($slacks.hold)
host_identify_commands=1_MAX
first_physical_read_discarded=YES
public_spaces=GB_IDENTIFY_ONLY
generic_cart_write=ABSENT
rom_read=ABSENT
save_access=ABSENT
nor_erase_program=ABSENT
mori_cfi_entry_00aa_then_0000=YES
gba_program81_rtl_regression=PASS
gba_readonly_source_sha256=$(Sha $gbaRead)
gba_programmer_source_sha256=$(Sha $gbaProgram)
os_sha256=$(Sha (Join-Path $common 'os.bin'))
rbf_sha256=$(Sha $rawRbf)
rbf_r_sha256=$(Sha (Join-Path $coreRoot 'chis.rbf_r'))
app_sha256=$(Sha (Join-Path $common 'chisburner91.elf'))
"@
[IO.File]::WriteAllText((Join-Path $bundle 'BUILD_PROVENANCE.txt'),$prov,$utf8)
Get-ChildItem $sd -Filter '*.json' -Recurse -File|ForEach-Object{$null=Get-Content -Raw $_|ConvertFrom-Json}
if(Get-ChildItem $sd -Recurse -File|Where-Object{$_.Extension-in@('.gb','.gbc','.gba')}){throw 'ROM unexpectedly included'}
$prefix=[IO.Path]::GetFullPath($sd).TrimEnd('\')+'\'
$sums=Get-ChildItem $sd -Recurse -File|Sort-Object FullName|ForEach-Object{"$(Sha $_.FullName)  $($_.FullName.Substring($prefix.Length).Replace('\','/'))"}
if($sums.Count-ne 14){throw "Unexpected SD file count: $($sums.Count)"}
[IO.File]::WriteAllLines((Join-Path $bundle 'SHA256SUMS.txt'),[string[]]$sums,$utf8)
Compress-Archive -Path (Join-Path $bundle '*') -DestinationPath $zip -CompressionLevel Optimal
"MBC5 identify91 package ready: $zip"
"ZIP SHA256: $(Sha $zip)"
