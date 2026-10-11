[CmdletBinding()]
param(
 [string]$Distro='Ubuntu-24.04',
 [string]$OpenFpgaSdk='/home/lin13/work/ONS_FPGA/openfpgaSDK',
 [string]$OsBuild='.integration/openfpgaos/src/firmware/os/bld/gb-mbc5-ident91-pocket-release',
 [string]$FpgaBuild='.integration/openfpgaos/src/fpga/targets/pocket/bld/chisburner-gb-mbc5-geom92-s2-win'
)
$ErrorActionPreference='Stop'
. (Join-Path $PSScriptRoot 'os-image-integrity.ps1')
function Sha([string]$p){(Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash.ToLowerInvariant()}
function Wsl([string]$c){& wsl.exe -d $Distro -- bash -lc $c; if($LASTEXITCODE){throw "Failed: $c"}}
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$wroot='/mnt/'+$root.Substring(0,1).ToLowerInvariant()+'/'+$root.Substring(3).Replace('\','/')
$base=Join-Path $root 'build/smoke/ChisBurner-smoke.91-gb-mbc5-nor-identify-r1/SD_ROOT'
$bundle=Join-Path $root 'build/smoke/ChisBurner-smoke.92-mx29gl640eb-geometry'
$zip="$bundle.zip"
$osRoot=if([IO.Path]::IsPathRooted($OsBuild)){$OsBuild}else{Join-Path $root $OsBuild}
$fpga=if([IO.Path]::IsPathRooted($FpgaBuild)){$FpgaBuild}else{Join-Path $root $FpgaBuild}
$os=Join-Path $osRoot 'os.bin'; $raw=Join-Path $fpga 'output_files/ap_core.rbf'
$qsf=Join-Path $fpga 'ap_core.qsf'; $sta=Join-Path $fpga 'output_files/ap_core.sta.rpt'
$appRoot=Join-Path $root 'app/gb_mbc5_geom92'; $app=Join-Path $root 'build/gb-mbc5-geom92-app/app.elf'
$rtl=Join-Path $root '.integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gb_mbc5_identify.v'
$utf8=[Text.UTF8Encoding]::new($false)
if((Test-Path $bundle)-or(Test-Path $zip)){throw 'Preserve existing smoke.92 package'}
foreach($p in @($base,$os,$raw,$qsf,$sta,$appRoot,$rtl)){if(!(Test-Path $p)){throw "Missing $p"}}
$q=Get-Content -Raw $qsf
if($q-notmatch'(?m)^set_global_assignment -name VERILOG_MACRO CHISBURNER_GB_MBC5_GEOM92\s*$'-or
   $q-match'(?m)^set_global_assignment -name VERILOG_MACRO CHISBURNER_GBA_PROGRAM65\s*$'){
    throw 'Unsafe FPGA profile selection'
}
$staText=Get-Content -Raw $sta; $slacks=@{}
foreach($kind in @('setup','hold')){
 $m=[regex]::Matches($staText,"Worst-case $kind slack is\s+(-?\d+(?:\.\d+)?)")
 if(!$m.Count){throw "Missing $kind timing"}
 $v=($m|ForEach-Object{[double]$_.Groups[1].Value}|Measure-Object -Minimum).Minimum
 if($v-le 0){throw "Timing failed: $kind $v"}; $slacks[$kind]=$v
}
$null=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes($os))
$src=Get-Content -Raw (Join-Path $appRoot 'main.c')
if($src-match'of_cart_(write|erase|program)' -or $src-notmatch'OF_CART_SPACE_IDENTIFY'){
 throw 'App escaped read-only identify contract'
}
Wsl "cd '$wroot' && rm -rf build/gb-geom92-package-rtl && verilator --cc --exe --build -j 4 -Wall -Wno-fatal --top-module chisburner_cart_gb_mbc5_identify -GGEOMETRY_PROFILE=1 -GADDRESS_SETUP_CYCLES=1 -GSELECT_SETUP_CYCLES=1 -GDATA_WAIT_CYCLES=1 -GRELEASE_CYCLES=1 -GWRITE_SETUP_CYCLES=1 -GWRITE_LOW_CYCLES=1 -GWRITE_RELEASE_CYCLES=1 -Mdir build/gb-geom92-package-rtl .integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gb_mbc5_identify.v '$wroot/tests/test-gb-mbc5-geom92-rtl.cpp' >/dev/null && build/gb-geom92-package-rtl/Vchisburner_cart_gb_mbc5_identify"
Wsl "cd '$wroot' && rm -rf build/gb-ident91-regress-rtl && verilator --cc --exe --build -j 4 -Wall -Wno-fatal --top-module chisburner_cart_gb_mbc5_identify -GADDRESS_SETUP_CYCLES=1 -GSELECT_SETUP_CYCLES=1 -GDATA_WAIT_CYCLES=1 -GRELEASE_CYCLES=1 -GWRITE_SETUP_CYCLES=1 -GWRITE_LOW_CYCLES=1 -GWRITE_RELEASE_CYCLES=1 -Mdir build/gb-ident91-regress-rtl .integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gb_mbc5_identify.v '$wroot/tests/test-gb-mbc5-ident91-rtl.cpp' >/dev/null && build/gb-ident91-regress-rtl/Vchisburner_cart_gb_mbc5_identify"
Wsl "cd '$wroot' && rm -rf build/gba-geom92-regress-rtl && verilator --cc --exe --build -j 4 -Wall -Wno-fatal --top-module chisburner_cart_gba_s29gl256 -Mdir build/gba-geom92-regress-rtl .integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gba_readonly.v .integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gba_s29gl256.v '$wroot/tests/test-program65-rtl.cpp' >/dev/null && build/gba-geom92-regress-rtl/Vchisburner_cart_gba_s29gl256"
Wsl "cd '$wroot/app/gb_mbc5_geom92' && make -B all OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0"
if(!(Test-Path $app)){throw 'App build missing'}
if(-not('ChisBurner.Gb92.Reverse'-as[type])){Add-Type -TypeDefinition @'
namespace ChisBurner.Gb92 { public static class Reverse { public static byte[] Apply(byte[] i) { var t=new byte[256]; for(int n=0;n<256;n++){int x=n,y=0;for(int b=0;b<8;b++){y=(y<<1)|(x&1);x>>=1;}t[n]=(byte)y;}var o=new byte[i.Length];for(int n=0;n<i.Length;n++)o[n]=t[i[n]];return o; } } }
'@}
$sd=Join-Path $bundle 'SD_ROOT'; New-Item -ItemType Directory -Path $bundle,$sd|Out-Null
$prefix=[IO.Path]::GetFullPath($base).TrimEnd('\')+'\'
foreach($f in Get-ChildItem $base -Recurse -File){
 $rel=$f.FullName.Substring($prefix.Length).Replace('\','/')
 if($rel-in@('Assets/chisburner/common/chisburner91.elf','Assets/chisburner/common/chisburner91.ini','Cores/ChisBurner.ChisBurner/chis.rbf_r')){continue}
 $d=Join-Path $sd $rel; New-Item -ItemType Directory -Force (Split-Path $d -Parent)|Out-Null; Copy-Item $f.FullName $d
}
$common=Join-Path $sd 'Assets/chisburner/common'; $coreRoot=Join-Path $sd 'Cores/ChisBurner.ChisBurner'
Copy-Item $os (Join-Path $common 'os.bin') -Force; Copy-Item $app (Join-Path $common 'chisburner92.elf')
[IO.File]::WriteAllText((Join-Path $common 'chisburner92.ini'),"[os]`nELF=chisburner92.elf`nARGS=`n",$utf8)
[IO.File]::WriteAllBytes((Join-Path $coreRoot 'chis.rbf_r'),[ChisBurner.Gb92.Reverse]::Apply([IO.File]::ReadAllBytes($raw)))
$cp=Join-Path $coreRoot 'core.json'; $core=Get-Content -Raw $cp|ConvertFrom-Json
$core.core.metadata.description='GB/GBC MX29GL640EB repeated CFI geometry diagnostic'
$core.core.metadata.version='0.1.0-smoke.92-mx29gl640eb-geometry'; $core.core.metadata.date_release='2026-09-07'
if($core.core.framework.hardware.cartridge_adapter-ne 16777216){throw 'Play Cartridge mode lost'}
[IO.File]::WriteAllText($cp,($core|ConvertTo-Json -Depth 20),$utf8)
$dp=Join-Path $coreRoot 'data.json'; $data=Get-Content -Raw $dp|ConvertFrom-Json
($data.data.data_slots|Where-Object{$_.id-eq 2}).filename='chisburner92.ini'
($data.data.data_slots|Where-Object{$_.id-eq 3}).filename='chisburner92.elf'
[IO.File]::WriteAllText($dp,($data|ConvertTo-Json -Depth 20),$utf8)
[IO.File]::WriteAllText((Join-Path $coreRoot 'info.txt'),'smoke.92: repeated MX29GL640EB CFI geometry and PRI bottom-boot identification only; no erase/program/ROM/SRAM; GBA unchanged.',$utf8)
$readme=@"
ChisBurner - smoke.92 MX29GL640EB 完整几何只读确认

本版仍不擦除、不烧写，也不访问普通 ROM 或 SRAM。它固定执行与 smoke.91 相同的
JEDEC/CFI 识别流程，随后把两个 CFI 擦除区描述符连续读取两遍，并读取 PRI1.1
Bottom Boot 标志。应用不能提交写地址或数据。

安装：关机，把 SD_ROOT 合并到 SD 卡根目录并覆盖旧文件；保持同一张 MBC5 烧录卡，
启动 Play Cartridge。第 1 页 CAPS/SAFE START 应 PASS，按 A 一次。请拍摄第 3、4 页。
通过标准：JEDEC MX29GL640EB PASS；GEOM REPEAT PASS；EXPECT PASS；REG0 8 x 8 KiB；
REG1 127 x 64 KiB；TOTAL 8192 KiB PASS；PRI1.1 BOTTOM BOOT PASS；
SIZE/BUFFER/REGIONS PASS；FINAL/SAFE PASS；DECISION PROFILE READY。
任何一项不符都停止，不进入擦除阶段。

GBA 引擎未改动并通过 RTL 回归。Quartus seed 2 全编译通过：
setup $($slacks.setup) ns，hold $($slacks.hold) ns。
"@
[IO.File]::WriteAllText((Join-Path $bundle 'README_TEST92_CN.txt'),$readme,$utf8)
$prov="version=0.1.0-smoke.92-mx29gl640eb-geometry`nquartus_seed=2`nsetup_slack_ns=$($slacks.setup)`nhold_slack_ns=$($slacks.hold)`npublic_space=GB_IDENTIFY_ONLY`nnor_erase_program=ABSENT`ngeneric_write=ABSENT`nrom_sram_access=ABSENT`ngeometry_expected=8x8KiB+127x64KiB`nbuffer_expected=32B`nboot_expected=BOTTOM`ngba_rtl_regression=PASS`n"
[IO.File]::WriteAllText((Join-Path $bundle 'BUILD_PROVENANCE.txt'),$prov,$utf8)
Get-ChildItem $sd -Filter '*.json' -Recurse|ForEach-Object{$null=Get-Content -Raw $_|ConvertFrom-Json}
if(Get-ChildItem $sd -Recurse -File|Where-Object{$_.Extension-in@('.gb','.gbc','.gba')}){throw 'ROM included'}
$prefix=[IO.Path]::GetFullPath($sd).TrimEnd('\')+'\'
$sums=Get-ChildItem $sd -Recurse -File|Sort-Object FullName|ForEach-Object{"$(Sha $_.FullName)  $($_.FullName.Substring($prefix.Length).Replace('\','/'))"}
if($sums.Count-ne 14){throw "Unexpected SD file count $($sums.Count)"}
[IO.File]::WriteAllLines((Join-Path $bundle 'SHA256SUMS.txt'),[string[]]$sums,$utf8)
Compress-Archive -Path (Join-Path $bundle '*') -DestinationPath $zip -CompressionLevel Optimal
"MBC5 geometry92 package ready: $zip"
"ZIP SHA256: $(Sha $zip)"
