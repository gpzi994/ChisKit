[CmdletBinding()]
param(
    [string]$Distro='Ubuntu-24.04',
    [string]$OpenFpgaSdk='/home/lin13/work/ONS_FPGA/openfpgaSDK',
    [string]$FpgaBuild='.integration/openfpgaos/src/fpga/targets/pocket/bld/chisburner-gb-mapper-auto93-s2-win'
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
$baseBundle=Join-Path $root 'build/smoke/ChisBurner-smoke.90-gb-mbc5-first-read-discard'
$base=Join-Path $baseBundle 'SD_ROOT'
$bundle=Join-Path $root 'build/smoke/ChisBurner-smoke.93-gb-mapper-auto'
$zip="$bundle.zip"
$fpga=if([IO.Path]::IsPathRooted($FpgaBuild)){$FpgaBuild}else{Join-Path $root $FpgaBuild}
$rawRbf=Join-Path $fpga 'output_files/ap_core.rbf'
$fpgaManifest=Join-Path $fpga 'GB_MAPPER_AUTO93_SHA256.txt'
$qsf=Join-Path $fpga 'ap_core.qsf'
$sta=Join-Path $fpga 'output_files/ap_core.sta.rpt'
$appRoot=Join-Path $root 'app/gb_mapper_auto93'
$app=Join-Path $root 'build/gb-mapper-auto93-app/app.elf'
$gbRtl=Join-Path $root '.integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gb_mbc5_fullrom.v'
$coreTop=Join-Path $root '.integration/openfpgaos/src/fpga/targets/pocket/core_top.v'
$gbaReadRtl=Join-Path $root '.integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gba_readonly.v'
$gbaProgramRtl=Join-Path $root '.integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gba_s29gl256.v'
$utf8=[Text.UTF8Encoding]::new($false)

if((Test-Path -LiteralPath $bundle) -or (Test-Path -LiteralPath $zip)){
    throw 'Preserve existing smoke.93 package'
}
foreach($p in @($baseBundle,$base,$rawRbf,$fpgaManifest,$qsf,$sta,$appRoot,
                  $gbRtl,$coreTop,$gbaReadRtl,$gbaProgramRtl)){
    if(!(Test-Path -LiteralPath $p)){throw "Missing package input: $p"}
}
foreach($line in Get-Content -LiteralPath (Join-Path $baseBundle 'SHA256SUMS.txt')){
    if($line -notmatch '^([0-9a-f]{64})  (.+)$'){throw "Malformed smoke.90 sum: $line"}
    $p=Join-Path $base $Matches[2]
    if(!(Test-Path -LiteralPath $p) -or (Sha $p) -cne $Matches[1]){
        throw "smoke.90 package drift: $($Matches[2])"
    }
}
foreach($line in Get-Content -LiteralPath $fpgaManifest){
    if($line -notmatch '^([0-9a-f]{64})  (.+)$'){throw "Malformed FPGA sum: $line"}
    if(!(Test-Path -LiteralPath $Matches[2]) -or (Sha $Matches[2]) -cne $Matches[1]){
        throw "FPGA source drift: $($Matches[2])"
    }
}

$qsfText=Get-Content -Raw -LiteralPath $qsf
if($qsfText -notmatch '(?m)^set_global_assignment -name VERILOG_MACRO CHISBURNER_GB_MAPPER_AUTO93\s*$' -or
   $qsfText -match '(?m)^set_global_assignment -name VERILOG_MACRO CHISBURNER_GBA_PROGRAM65\s*$'){
    throw 'FPGA build is not isolated mapper-auto93'
}
$staText=Get-Content -Raw -LiteralPath $sta
$slacks=@{}
foreach($kind in @('setup','hold')){
    $m=[regex]::Matches($staText,"Worst-case $kind slack is\s+(-?\d+(?:\.\d+)?)")
    if(!$m.Count){throw "Missing $kind timing result"}
    $v=($m|ForEach-Object{[double]$_.Groups[1].Value}|Measure-Object -Minimum).Minimum
    if($v -le 0){throw "Refuse package: $kind slack $v ns"}
    $slacks[$kind]=$v
}

$baseOs=Join-Path $base 'Assets/chisburner/common/os.bin'
$baseOsHash=Sha $baseOs
$null=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes($baseOs))
$appSource=Get-Content -Raw -LiteralPath (Join-Path $appRoot 'main.c')
if($appSource -match 'of_cart_(write|erase|program)'){
    throw 'mapper-auto93 app contains cartridge mutation call'
}
$gbSource=Get-Content -Raw -LiteralPath $gbRtl
if($gbSource -notmatch 'MAPPER_AUTO_PROFILE' -or
   $gbSource -notmatch "gb_address\s*=\s*16'h2000" -or
   $gbSource -notmatch "target_bank\s*<=\s*\(MAPPER_AUTO_PROFILE" -or
   $gbSource -match "16'h3000"){
    throw 'Mapper probe structural whitelist missing'
}

Wsl "cd '$wroot/app/gb_mapper_auto93' && make -B all OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0"
foreach($exe in @('build/gb-auto93-mbc3-rtl/Vchisburner_cart_gb_mbc5_fullrom',
                  'build/gb-auto93-mbc5-rtl/Vchisburner_cart_gb_mbc5_fullrom',
                  'build/gb-auto93-default-rtl/Vchisburner_cart_gb_mbc5_fullrom',
                  'build/gba-auto93-regression-rtl/Vchisburner_cart_gba_s29gl256')){
    Wsl "cd '$wroot' && '$wroot/$exe'"
}
if(!(Test-Path -LiteralPath $app)){throw 'mapper-auto93 app build missing'}

if(-not ('ChisBurner.Gb93.Reverse' -as [type])){
    Add-Type -TypeDefinition @'
namespace ChisBurner.Gb93 {
 public static class Reverse {
  public static byte[] Apply(byte[] input) {
   var table=new byte[256];
   for(int i=0;i<256;i++){int x=i,y=0;for(int b=0;b<8;b++){y=(y<<1)|(x&1);x>>=1;}table[i]=(byte)y;}
   var output=new byte[input.Length];
   for(int i=0;i<input.Length;i++)output[i]=table[input[i]];
   return output;
  }
 }
}
'@
}

$sd=Join-Path $bundle 'SD_ROOT'
New-Item -ItemType Directory -Path $bundle,$sd|Out-Null
$basePrefix=[IO.Path]::GetFullPath($base).TrimEnd('\')+'\'
foreach($file in Get-ChildItem -LiteralPath $base -Recurse -File){
    $rel=$file.FullName.Substring($basePrefix.Length).Replace('\','/')
    if($rel -in @('Assets/chisburner/common/chisburner90.elf',
                  'Assets/chisburner/common/chisburner90.ini',
                  'Cores/ChisBurner.ChisBurner/chis.rbf_r')){continue}
    $dest=Join-Path $sd $rel
    New-Item -ItemType Directory -Force (Split-Path $dest -Parent)|Out-Null
    Copy-Item -LiteralPath $file.FullName -Destination $dest
}
$common=Join-Path $sd 'Assets/chisburner/common'
$coreRoot=Join-Path $sd 'Cores/ChisBurner.ChisBurner'
Copy-Item -LiteralPath $app -Destination (Join-Path $common 'chisburner93.elf')
[IO.File]::WriteAllText((Join-Path $common 'chisburner93.ini'),"[os]`nELF=chisburner93.elf`nARGS=`n",$utf8)
[IO.File]::WriteAllBytes((Join-Path $coreRoot 'chis.rbf_r'),
 [ChisBurner.Gb93.Reverse]::Apply([IO.File]::ReadAllBytes($rawRbf)))

$corePath=Join-Path $coreRoot 'core.json'
$core=Get-Content -Raw -LiteralPath $corePath|ConvertFrom-Json
$core.core.metadata.description='Read-only automatic MBC3/MBC5 behavioral and header detector'
$core.core.metadata.version='0.1.0-smoke.93-gb-mapper-auto'
$core.core.metadata.date_release='2026-09-07'
if($core.core.framework.hardware.cartridge_adapter -ne 16777216){throw 'Play Cartridge mode lost'}
[IO.File]::WriteAllText($corePath,($core|ConvertTo-Json -Depth 20),$utf8)
$dataPath=Join-Path $coreRoot 'data.json'
$data=Get-Content -Raw -LiteralPath $dataPath|ConvertFrom-Json
($data.data.data_slots|Where-Object id -eq 2).filename='chisburner93.ini'
($data.data.data_slots|Where-Object id -eq 3).filename='chisburner93.elf'
[IO.File]::WriteAllText($dataPath,($data|ConvertTo-Json -Depth 20),$utf8)
[IO.File]::WriteAllText((Join-Path $coreRoot 'info.txt'),
 'smoke.93: read-only Mori-compatible automatic MBC3/MBC5 behavior plus header agreement; GBA unchanged.', $utf8)
if((Sha (Join-Path $common 'os.bin')) -ne $baseOsHash){throw 'Reused smoke.90 OS changed'}

$readme=@"
ChisBurner - smoke.93 GB/GBC MBC3/MBC5 自动识别

本阶段只读，不执行 NOR/Flash 擦除或烧写，也不访问 SRAM/存档。
识别方法与 Mori 项目的已验证流程一致：向 0x2000 写入 ROM bank 0，分别读取
固定窗口 0x0000 和切换窗口 0x4000。两窗口相同判为 MBC5；不同判为 MBC3。
整个行为测试重复两遍，并读取完整卡头；卡头类型必须与行为识别一致才会通过。

测试步骤：
1. 把 SD_ROOT 合并到 SD 卡根目录并覆盖旧文件。
2. 完全关机，插入第一张卡，启动 Play Cartridge，按 A 一次。
3. 拍摄第 3、4 页。
4. 完全关机后再更换第二张卡，重复测试；禁止热插拔。

合格标准：两张卡分别显示 MBC3 和 MBC5；第 4 页 MAPPER AGREEMENT、
LOGO/CHECKSUM、REPEAT/SAMPLES、FINAL/SAFE 均 PASS，DECISION 为
AUTO PROFILE READY。
"@
[IO.File]::WriteAllText((Join-Path $bundle 'README_TEST93_CN.txt'),$readme,$utf8)

$provenance=@"
version=0.1.0-smoke.93-gb-mapper-auto
quartus_full_compile=PASS
quartus_seed=2
minimum_setup_slack_ns=$($slacks.setup)
minimum_hold_slack_ns=$($slacks.hold)
base_os_exact_reuse=YES
os_sha256=$baseOsHash
mapper_method=MORI_BANK0_FIXED_VS_SWITCH
mapper_probe_passes=2
header_bytes=96
first_physical_block_discarded=YES
mapper_write_address=0x2000_ONLY
mapper_write_values=0x00_OR_0x01_ONLY
save_access=ABSENT
nor_erase_program=ABSENT
mbc3_behavior_rtl=PASS
mbc5_behavior_rtl=PASS
mbc5_fullrom_regression=PASS
gba_program81_rtl_regression=PASS
gba_readonly_source_sha256=$(Sha $gbaReadRtl)
gba_programmer_source_sha256=$(Sha $gbaProgramRtl)
rbf_sha256=$(Sha $rawRbf)
rbf_r_sha256=$(Sha (Join-Path $coreRoot 'chis.rbf_r'))
app_sha256=$(Sha (Join-Path $common 'chisburner93.elf'))
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
if($sums.Count -ne 14){throw "Unexpected SD file count: $($sums.Count)"}
[IO.File]::WriteAllLines((Join-Path $bundle 'SHA256SUMS.txt'),[string[]]$sums,$utf8)
Compress-Archive -Path (Join-Path $bundle '*') -DestinationPath $zip -CompressionLevel Optimal
"GB mapper auto93 package ready: $zip"
"ZIP SHA256: $(Sha $zip)"
