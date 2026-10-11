param([int]$Seed=5)
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$utf8=[Text.UTF8Encoding]::new($false)
function Sha($p){(Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash.ToLowerInvariant()}
. (Join-Path $PSScriptRoot 'os-image-integrity.ps1')
$base=Join-Path $root 'build/smoke/ChisBurner-smoke.96-load-fix'
$bundle=Join-Path $root 'build/smoke/ChisBurner-smoke.97-ui-stage1'
$fpga=Join-Path $root ".integration/openfpgaos/src/fpga/targets/pocket/bld/chisburner-ui97-s$Seed-win"
$os=Join-Path $root '.integration/openfpgaos/src/firmware/os/bld/ui97-pocket/os.bin'
$app=Join-Path $root 'build/ui97-app/app.elf'
if((Test-Path $bundle)-or(Test-Path "$bundle.zip")){throw 'Preserve existing UI97 package'}
foreach($line in Get-Content (Join-Path $root 'build/ui97-tested-sources.sha256')){
 if($line -notmatch '^([0-9a-f]{64})  (.+)$'){throw 'Malformed test receipt'}
 if((Sha (Join-Path $root $Matches[2])) -cne $Matches[1]){throw 'Retest changed sources before packaging'}
}
foreach($line in Get-Content (Join-Path $root 'build/ui97-app-sources.sha256')){
 if($line -notmatch '^([0-9a-f]{64})  (.+)$'){throw 'Malformed application build receipt'}
 if((Sha (Join-Path $root $Matches[2])) -cne $Matches[1]){throw 'Rebuild application after source changes'}
}
foreach($line in Get-Content (Join-Path $base 'SHA256SUMS.txt')){
 if($line -notmatch '^([0-9a-f]{64})  (.+)$'){throw 'Malformed baseline manifest'}
 if((Sha (Join-Path "$base/SD_ROOT" $Matches[2])) -cne $Matches[1]){throw 'Accepted baseline drift'}
}
$q=Get-Content (Join-Path $fpga 'ap_core.qsf') -Raw
if($q -notmatch '(?m)^set_global_assignment -name VERILOG_MACRO CHISBURNER_UI97\s*$'){throw 'Wrong FPGA profile'}
$sta=Get-Content (Join-Path $fpga 'output_files/ap_core.sta.rpt') -Raw
$slack=@{}
foreach($kind in @('setup','hold','recovery','removal','minimum pulse width')){
 $m=[regex]::Matches($sta,"Worst-case $kind slack is\s+(-?\d+(?:\.\d+)?)")
 if(!$m.Count){throw "No $kind report"}
 $v=($m|ForEach-Object{[double]$_.Groups[1].Value}|Measure-Object -Minimum).Minimum
 if($v -le 0){throw "Refuse timing-failed bitstream: $kind $v ns"}
 $slack[$kind]=$v
}
if((Sha (Join-Path $fpga 'firmware.mif')) -ne (Sha (Join-Path (Split-Path $os) 'firmware.mif'))){throw 'Boot MIF drift'}
$integrity=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes($os))
$appBytes=[IO.File]::ReadAllBytes($app)
if($appBytes[0] -ne 127 -or [Text.Encoding]::ASCII.GetString($appBytes,1,3) -cne 'ELF' -or $appBytes[18] -ne 243){throw 'App is not RISC-V ELF'}
if((Get-Content (Join-Path $root 'app/ui97/main.c') -Raw) -match 'of_cart_(write|erase|program|flash)'){throw 'Stage1 has mutation/NOR calls'}
Add-Type -TypeDefinition @'
namespace ChisBurner.UI97 {
 public static class Reverse {
  public static byte[] Apply(byte[] input) {
   var table=new byte[256];
   for(int i=0;i<256;i++){int x=i,y=0;for(int b=0;b<8;b++){y=(y<<1)|(x&1);x>>=1;}table[i]=(byte)y;}
   var output=new byte[input.Length];for(int i=0;i<input.Length;i++)output[i]=table[input[i]];return output;
  }
 }
}
'@
$sd=Join-Path $bundle 'SD_ROOT'
$prefix=[IO.Path]::GetFullPath("$base/SD_ROOT").TrimEnd('\')+'\'
foreach($f in Get-ChildItem "$base/SD_ROOT" -Recurse -File){
 $rel=$f.FullName.Substring($prefix.Length)
 if($f.Name -in @('cb96.elf','cb96.ini')){continue}
 $dest=Join-Path $sd $rel
 New-Item -ItemType Directory -Force (Split-Path $dest) | Out-Null
 Copy-Item -LiteralPath $f.FullName -Destination $dest
}
$common=Join-Path $sd 'Assets/chisburner/common'
$coreRoot=Join-Path $sd 'Cores/ChisBurner.ChisBurner'
Copy-Item $os "$common/os.bin" -Force
Copy-Item $app "$common/cb97.elf"
[IO.File]::WriteAllText("$common/cb97.ini","[os]`nELF=cb97.elf`nARGS=`n",$utf8)
$rbf=Join-Path $fpga 'output_files/ap_core.rbf'
[IO.File]::WriteAllBytes("$coreRoot/chis.rbf_r",[ChisBurner.UI97.Reverse]::Apply([IO.File]::ReadAllBytes($rbf)))
$core=Get-Content "$coreRoot/core.json" -Raw|ConvertFrom-Json
$core.core.metadata.description='Three-page UI and read-only GB/GBC or GBA header detection'
$core.core.metadata.version='0.1.0-smoke.97-ui-stage1'
$core.core.metadata.date_release='2026-09-07'
if($core.core.framework.hardware.cartridge_adapter -ne 16777216){throw 'Play Cartridge mode changed'}
foreach($s in @($core.core.metadata.platform_ids[0],$core.core.cores[0].name,$core.core.cores[0].filename)){
 if($s.Length -gt 15){throw 'Pocket identifier length exceeds 15'}
}
if((Split-Path $coreRoot -Leaf) -cne "$($core.core.metadata.author).$($core.core.metadata.shortname)"){throw 'Core directory mismatch'}
[IO.File]::WriteAllText("$coreRoot/core.json",($core|ConvertTo-Json -Depth 20),$utf8)
$data=Get-Content "$coreRoot/data.json" -Raw|ConvertFrom-Json
($data.data.data_slots|Where-Object id -eq 2).filename='cb97.ini'
($data.data.data_slots|Where-Object id -eq 3).filename='cb97.elf'
($data.data.data_slots|Where-Object id -eq 4).name='Unused in UI stage 1'
($data.data.data_slots|Where-Object id -eq 4).extensions=@('gba','gb','gbc')
[IO.File]::WriteAllText("$coreRoot/data.json",($data|ConvertTo-Json -Depth 20),$utf8)
[IO.File]::WriteAllText("$coreRoot/info.txt",'smoke.97: UI stage 1; read-only GB/GBC or GBA headers. No ROM/save transfer or NOR commands. Power off before swapping cartridges.',$utf8)
Copy-Item (Join-Path $root 'app/ui97/README_TEST_CN.md') (Join-Path $bundle 'README_TEST_CN.md')
New-Item -ItemType Directory (Join-Path $bundle 'PREVIEW')|Out-Null
Copy-Item (Join-Path $root 'build/ui97-preview/*.png') (Join-Path $bundle 'PREVIEW')
$provenance=@(
 'version=0.1.0-smoke.97-ui-stage1',"seed=$Seed",'quartus_full_compile=PASS',
 "setup_ns=$($slack.setup)","hold_ns=$($slack.hold)",
 'host_asan_ubsan=PASS','dual_bus_rtl=PASS','gba_program_regression=PASS','mbc3_mbc5_regression=PASS',
 'hardware_test=PENDING','nor_identification=NOT_ENABLED','transfers=NOT_ENABLED',
 'reference_rom_included=NO',"raw_rbf_sha256=$(Sha $rbf)","os_sha256=$(Sha $os)","app_sha256=$(Sha $app)",
 'os_flags=CART_MMIO_ENABLED=1; generic read API; no programmer',
 'contract_stamp=05834c702f853ee435e9062b4909b5e1; refreshed for existing additive cartridge ABI fields; no interface changes by UI97'
)
[IO.File]::WriteAllLines((Join-Path $bundle 'BUILD_PROVENANCE.txt'),[string[]]$provenance,$utf8)
$sources=@('app/ui97/main.c','app/ui97/model.c','app/ui97/model.h','app/ui97/ui.c','app/ui97/ui.h','app/ui97/font.c','app/ui97/Makefile','core/rtl/chisburner_cart_auto97.v',
 '.integration/openfpgaos/src/fpga/targets/pocket/core_top.v','.integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gba_readonly.v',
 '.integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gba_s29gl256.v','.integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gb_readonly.v',
 '.integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gb_readonly_abstract.v','.integration/openfpgaos/src/firmware/os/kernel/services_table.c',
 'tests/test-ui97.c','tests/test-ui97-rtl.cpp','tests/build-ui97-fpga.ps1','tests/build-ui97-package.ps1')
[IO.File]::WriteAllLines((Join-Path $bundle 'SOURCES_SHA256.txt'),[string[]]@($sources|ForEach-Object{"$(Sha (Join-Path $root $_))  $_"}),$utf8)
foreach($f in Get-ChildItem $sd -Recurse -Filter '*.json'){$null=Get-Content $f.FullName -Raw|ConvertFrom-Json}
foreach($slot in $data.data.data_slots){if($slot.filename -and !(Test-Path (Join-Path $common $slot.filename))){throw "Missing slot payload $($slot.filename)"}}
$prefix=[IO.Path]::GetFullPath($sd).TrimEnd('\')+'\'
$sums=@(Get-ChildItem $sd -Recurse -File|Sort-Object FullName|ForEach-Object{"$(Sha $_.FullName)  $($_.FullName.Substring($prefix.Length).Replace('\','/'))"})
[IO.File]::WriteAllLines((Join-Path $bundle 'SHA256SUMS.txt'),[string[]]$sums,$utf8)
Compress-Archive -Path "$bundle/*" -DestinationPath "$bundle.zip" -CompressionLevel Optimal
Write-Output "UI97 package: $bundle.zip"
