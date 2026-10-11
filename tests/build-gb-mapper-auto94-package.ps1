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
$baseBundle=Join-Path $root 'build/smoke/ChisBurner-smoke.93-gb-mapper-auto'
$base=Join-Path $baseBundle 'SD_ROOT'
$bundle=Join-Path $root 'build/smoke/ChisBurner-smoke.94-gb-header-canonical'
$zip="$bundle.zip"
$fpga=if([IO.Path]::IsPathRooted($FpgaBuild)){$FpgaBuild}else{Join-Path $root $FpgaBuild}
$fpgaManifest=Join-Path $fpga 'GB_MAPPER_AUTO93_SHA256.txt'
$qsf=Join-Path $fpga 'ap_core.qsf'
$sta=Join-Path $fpga 'output_files/ap_core.sta.rpt'
$appRoot=Join-Path $root 'app/gb_mapper_auto94'
$app=Join-Path $root 'build/gb-mapper-auto94-app/app.elf'
$gbRtl=Join-Path $root '.integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gb_mbc5_fullrom.v'
$gbaReadRtl=Join-Path $root '.integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gba_readonly.v'
$gbaProgramRtl=Join-Path $root '.integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gba_s29gl256.v'
$utf8=[Text.UTF8Encoding]::new($false)

if((Test-Path -LiteralPath $bundle) -or (Test-Path -LiteralPath $zip)){
    throw 'Preserve existing smoke.94 package'
}
foreach($p in @($baseBundle,$base,$fpgaManifest,$qsf,$sta,$appRoot,$gbRtl,$gbaReadRtl,$gbaProgramRtl)){
    if(!(Test-Path -LiteralPath $p)){throw "Missing package input: $p"}
}
foreach($line in Get-Content -LiteralPath (Join-Path $baseBundle 'SHA256SUMS.txt')){
    if($line -notmatch '^([0-9a-f]{64})  (.+)$'){throw "Malformed smoke.93 sum: $line"}
    $p=Join-Path $base $Matches[2]
    if(!(Test-Path -LiteralPath $p) -or (Sha $p) -cne $Matches[1]){
        throw "smoke.93 package drift: $($Matches[2])"
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

$baseRbf=Join-Path $base 'Cores/ChisBurner.ChisBurner/chis.rbf_r'
$baseRbfHash=Sha $baseRbf
$baseOs=Join-Path $base 'Assets/chisburner/common/os.bin'
$baseOsHash=Sha $baseOs
$null=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes($baseOs))
$appSource=(Get-Content -Raw -LiteralPath (Join-Path $appRoot 'main.c'))+
           (Get-Content -Raw -LiteralPath (Join-Path $appRoot 'mapper_decision.c'))
if($appSource -match 'of_cart_(write|erase|program)'){
    throw 'mapper-auto94 app contains cartridge mutation call'
}

Wsl "cd '$wroot/app/gb_mapper_auto94' && make -B all OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0"
Wsl "cd '$wroot' && gcc -std=c99 -Wall -Wextra -Werror -Iapp/gb_mapper_auto94 tests/test-gb-mapper-decision94.c app/gb_mapper_auto94/mapper_decision.c -o build/test-gb-mapper-decision94 && build/test-gb-mapper-decision94"
foreach($exe in @('build/gb-auto93-mbc3-rtl/Vchisburner_cart_gb_mbc5_fullrom',
                  'build/gb-auto93-mbc5-rtl/Vchisburner_cart_gb_mbc5_fullrom',
                  'build/gb-auto93-default-rtl/Vchisburner_cart_gb_mbc5_fullrom',
                  'build/gba-auto93-regression-rtl/Vchisburner_cart_gba_s29gl256')){
    Wsl "cd '$wroot' && '$wroot/$exe'"
}
if(!(Test-Path -LiteralPath $app)){throw 'mapper-auto94 app build missing'}

$sd=Join-Path $bundle 'SD_ROOT'
New-Item -ItemType Directory -Path $bundle,$sd|Out-Null
$basePrefix=[IO.Path]::GetFullPath($base).TrimEnd('\')+'\'
foreach($file in Get-ChildItem -LiteralPath $base -Recurse -File){
    $rel=$file.FullName.Substring($basePrefix.Length).Replace('\','/')
    if($rel -in @('Assets/chisburner/common/chisburner93.elf','Assets/chisburner/common/chisburner93.ini')){continue}
    $dest=Join-Path $sd $rel
    New-Item -ItemType Directory -Force (Split-Path $dest -Parent)|Out-Null
    Copy-Item -LiteralPath $file.FullName -Destination $dest
}
$common=Join-Path $sd 'Assets/chisburner/common'
$coreRoot=Join-Path $sd 'Cores/ChisBurner.ChisBurner'
Copy-Item -LiteralPath $app -Destination (Join-Path $common 'chisburner94.elf')
[IO.File]::WriteAllText((Join-Path $common 'chisburner94.ini'),"[os]`nELF=chisburner94.elf`nARGS=`n",$utf8)

$corePath=Join-Path $coreRoot 'core.json'
$core=Get-Content -Raw -LiteralPath $corePath|ConvertFrom-Json
$core.core.metadata.description='Read-only header-canonical automatic MBC3/MBC5 detector'
$core.core.metadata.version='0.1.0-smoke.94-gb-header-canonical'
$core.core.metadata.date_release='2026-09-07'
if($core.core.framework.hardware.cartridge_adapter -ne 16777216){throw 'Play Cartridge mode lost'}
[IO.File]::WriteAllText($corePath,($core|ConvertTo-Json -Depth 20),$utf8)
$dataPath=Join-Path $coreRoot 'data.json'
$data=Get-Content -Raw -LiteralPath $dataPath|ConvertFrom-Json
($data.data.data_slots|Where-Object id -eq 2).filename='chisburner94.ini'
($data.data.data_slots|Where-Object id -eq 3).filename='chisburner94.elf'
[IO.File]::WriteAllText($dataPath,($data|ConvertTo-Json -Depth 20),$utf8)
[IO.File]::WriteAllText((Join-Path $coreRoot 'info.txt'),
 'smoke.94: valid GB header selects MBC3/MBC5; bank-0 probe diagnoses aliases; GBA unchanged.', $utf8)
if((Sha (Join-Path $common 'os.bin')) -ne $baseOsHash){throw 'Reused OS changed'}
if((Sha (Join-Path $coreRoot 'chis.rbf_r')) -ne $baseRbfHash){throw 'Reused FPGA image changed'}

$readme=@"
ChisBurner - smoke.94 GB/GBC 卡头主判定自动识别

smoke.93 已准确测出实体行为：这张 MBC3 烧录卡允许 ROM bank 0 镜像，因而
Mori 的“bank 0 相同即 MBC5”经验判据会与有效卡头 TYPE 10 冲突。

smoke.94 以 Nintendo Logo、卡头校验和均正确的卡头类型选择功能映射器；bank 0
测试保留为物理别名诊断。MBC3 的 ZERO ALIAS 与 ZERO->BANK1 均允许，MBC5
仍要求 ZERO ALIAS。全程只读；不执行 NOR/Flash 擦写，也不访问 SRAM/存档。

测试步骤：
1. 把 SD_ROOT 合并到 SD 卡根目录并覆盖旧文件。
2. 完全关机，插入 MBC3 卡，启动 Play Cartridge，按 A 一次，拍第 3、4 页。
3. 完全关机后换 MBC5 卡，重复；禁止热插拔。

合格标准：两张卡的 AUTO PROFILE 分别为 MBC3/MBC5；第 4 页
MAPPER PROFILE、LOGO/CHECKSUM、REPEAT/SAMPLES、FINAL/SAFE 均 PASS，
DECISION 为 AUTO PROFILE READY。
"@
[IO.File]::WriteAllText((Join-Path $bundle 'README_TEST94_CN.txt'),$readme,$utf8)

$provenance=@"
version=0.1.0-smoke.94-gb-header-canonical
fpga_exact_reuse_from_smoke93=YES
quartus_full_compile_smoke93=PASS
quartus_seed=2
minimum_setup_slack_ns=$($slacks.setup)
minimum_hold_slack_ns=$($slacks.hold)
os_sha256=$baseOsHash
rbf_r_sha256=$baseRbfHash
functional_mapper_source=VALIDATED_GB_HEADER
bank0_probe_role=PHYSICAL_ALIAS_DIAGNOSTIC
mbc3_bank0_equal=ACCEPTED
mbc3_bank0_different=ACCEPTED
mbc5_bank0_equal=ACCEPTED
mbc5_bank0_different=BLOCKED
first_physical_block_discarded=YES
save_access=ABSENT
nor_erase_program=ABSENT
decision_unit_test=PASS
mbc3_behavior_rtl=PASS
mbc5_behavior_rtl=PASS
mbc5_fullrom_regression=PASS
gba_program81_rtl_regression=PASS
gba_readonly_source_sha256=$(Sha $gbaReadRtl)
gba_programmer_source_sha256=$(Sha $gbaProgramRtl)
app_sha256=$(Sha (Join-Path $common 'chisburner94.elf'))
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
"GB mapper auto94 package ready: $zip"
"ZIP SHA256: $(Sha $zip)"
