param(
 [int]$Seed=11,
 [ValidateSet('standard','closure','floorplan','floorplan5','floorplan5control','phys','retime','control')][string]$Variant='standard'
)
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$utf8=[Text.UTF8Encoding]::new($false)
function Sha($p){(Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash.ToLowerInvariant()}
. (Join-Path $PSScriptRoot 'os-image-integrity.ps1')
$base=Join-Path $root 'build/smoke/ChisBurner-smoke.97-ui-stage1'
$bundle=Join-Path $root 'build/smoke/ChisBurner-smoke.101-transfer-mapper-fix'
$suffix=if($Variant -eq 'standard'){''}else{"-$Variant"}
$fpga=Join-Path $root ".integration/openfpgaos/src/fpga/targets/pocket/bld/chisburner-ui101-s$Seed$suffix-win"
$os=Join-Path $root '.integration/openfpgaos/src/firmware/os/bld/ui101-pocket/os.bin'
$app=Join-Path $root 'build/ui101-app/app.elf'
if((Test-Path $bundle)-or(Test-Path "$bundle.zip")){throw 'Preserve existing smoke.101 package'}
foreach($line in Get-Content (Join-Path $root 'build/ui101-tested-sources.sha256')){
 if($line -notmatch '^([0-9a-f]{64})  (.+)$'){throw 'Malformed test receipt'}
 if((Sha (Join-Path $root $Matches[2])) -cne $Matches[1]){throw 'Retest changed sources before packaging'}
}
foreach($line in Get-Content (Join-Path $root 'build/ui101-app-sources.sha256')){
 if($line -notmatch '^([0-9a-f]{64})  (.+)$'){throw 'Malformed application build receipt'}
 if((Sha (Join-Path $root $Matches[2])) -cne $Matches[1]){throw 'Rebuild application after source changes'}
}
foreach($line in Get-Content (Join-Path $base 'SHA256SUMS.txt')){
 if($line -notmatch '^([0-9a-f]{64})  (.+)$'){throw 'Malformed baseline manifest'}
 if((Sha (Join-Path "$base/SD_ROOT" $Matches[2])) -cne $Matches[1]){throw 'Accepted baseline drift'}
}
$q=Get-Content (Join-Path $fpga 'ap_core.qsf') -Raw
foreach($line in Get-Content (Join-Path $fpga 'UI98_INPUTS_SHA256.txt')){
 if($line -notmatch '^([0-9a-f]{64})  (.+)$'){throw 'Malformed FPGA input receipt'}
 if((Sha $Matches[2]) -cne $Matches[1]){throw 'FPGA source drift since compile'}
}
if($q -notmatch '(?m)^set_global_assignment -name VERILOG_MACRO CHISBURNER_UI98\s*$'){throw 'Wrong FPGA profile'}
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
Add-Type -TypeDefinition @'
namespace ChisBurner.UI98 {
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
 if($f.Name -in @('cb97.elf','cb97.ini')){continue}
 $dest=Join-Path $sd $rel
 New-Item -ItemType Directory -Force (Split-Path $dest) | Out-Null
 Copy-Item -LiteralPath $f.FullName -Destination $dest
}
$common=Join-Path $sd 'Assets/chisburner/common'
$coreRoot=Join-Path $sd 'Cores/ChisBurner.ChisBurner'
Copy-Item $os "$common/os.bin" -Force
Copy-Item $app "$common/cb101.elf"
[IO.File]::WriteAllText("$common/cb101.ini","[os]`nELF=cb101.elf`nARGS=`n",$utf8)
$rbf=Join-Path $fpga 'output_files/ap_core.rbf'
[IO.File]::WriteAllBytes("$coreRoot/chis.rbf_r",[ChisBurner.UI98.Reverse]::Apply([IO.File]::ReadAllBytes($rbf)))
$core=Get-Content "$coreRoot/core.json" -Raw|ConvertFrom-Json
$core.core.metadata.description='Stable dual-bus detection, repeatable NOR analysis and GBA transfer fixes'
$core.core.metadata.version='0.1.0-smoke.101-transfer-mapper-fix'
$core.core.metadata.date_release='2026-09-08'
if($core.core.framework.hardware.cartridge_adapter -ne 16777216){throw 'Play Cartridge mode changed'}
foreach($s in @($core.core.metadata.platform_ids[0],$core.core.cores[0].name,$core.core.cores[0].filename)){
 if($s.Length -gt 15){throw 'Pocket identifier length exceeds 15'}
}
if((Split-Path $coreRoot -Leaf) -cne "$($core.core.metadata.author).$($core.core.metadata.shortname)"){throw 'Core directory mismatch'}
[IO.File]::WriteAllText("$coreRoot/core.json",($core|ConvertTo-Json -Depth 20),$utf8)
$data=Get-Content "$coreRoot/data.json" -Raw|ConvertFrom-Json
($data.data.data_slots|Where-Object id -eq 2).filename='cb101.ini'
($data.data.data_slots|Where-Object id -eq 3).filename='cb101.elf'
$inputSlot=$data.data.data_slots|Where-Object id -eq 4
$inputSlot.name='ROM input (select .gba)'
$inputSlot.required=$false
$inputSlot.parameters=3
$inputSlot.deferload=$true
$inputSlot.extensions=@('gba')
$outputSlot=$data.data.data_slots|Where-Object id -eq 5
$outputSlot.name='Backup output (automatic)'
$outputSlot.required=$false
$outputSlot.parameters=0
$outputSlot.deferload=$true
$outputSlot.extensions=@('gba')
foreach($p in @('filename','nonvolatile','address','size_exact','size_maximum','secondary')){$outputSlot.PSObject.Properties.Remove($p)}
[IO.File]::WriteAllText("$coreRoot/data.json",($data|ConvertTo-Json -Depth 20),$utf8)
[IO.File]::WriteAllText("$coreRoot/info.txt",'smoke.101: 32 KiB backup target writes, NOR profile preserved into erase, repeatable MBC3/MBC5 analysis. GBA writes remain S29GL256S-only; GB NOR and all save writes disabled.',$utf8)
Copy-Item (Join-Path $root 'app/ui98/README_TEST_CN.md') (Join-Path $bundle 'README_TEST_CN.md')
New-Item -ItemType Directory (Join-Path $bundle 'PREVIEW')|Out-Null
Copy-Item (Join-Path $root 'build/ui98-preview/*.png') (Join-Path $bundle 'PREVIEW')
Copy-Item (Join-Path $root 'build/ui101-tested-sources.sha256') (Join-Path $bundle 'TESTED_SOURCES.txt')
$provenance=@(
 'version=0.1.0-smoke.101-transfer-mapper-fix',"seed=$Seed","fit_variant=$Variant",'quartus_full_compile=PASS',
 "setup_ns=$($slack.setup)","hold_ns=$($slack.hold)",
 'host_asan_ubsan=PASS','dual_bus_rtl=PASS','gba_program_regression=PASS','mbc3_mbc5_regression=PASS',
 'hardware_test=PENDING','gba_nor_write_whitelist=S29GL256S_ONLY','gb_nor_identification=MX29GL640EB_READ_ONLY','transfers=GBA_ROM_BACKUP;S29GL256S_PROGRAM;GB_AND_SAVES_LOCKED',
 'confirmation=first_A_selects;release_required;second_A_authorizes;B_cancels',
 'axi_router_87_checks=PASS','gb_geometry_rtl=PASS','program_32m_and_8705280_host=PASS','reference_rom_included=NO',"raw_rbf_sha256=$(Sha $rbf)","os_sha256=$(Sha $os)","app_sha256=$(Sha $app)",
 'backup_orchestration_mock=PASS;slot5_output;full32m;cancel;io_failure;corrupt_reread',
 'os_output_backend_mock=PASS;existing_file_refused;new_file;64k_split_to_2x32k;unexpected_result;io_errors',
 'os_flags=CART_MMIO_ENABLED=1;CART_UI98=1;CART_S29GL256_PROGRAMMER=1;CHISBURNER_OUTPUT_ENABLED=1',
 'contract_stamp=05834c702f853ee435e9062b4909b5e1; refreshed for existing additive cartridge ABI fields; no interface changes by UI98'
)
[IO.File]::WriteAllLines((Join-Path $bundle 'BUILD_PROVENANCE.txt'),[string[]]$provenance,$utf8)
$sources=@(
 'core/rtl/chisburner_cart_auto98.v',
 '.integration/openfpgaos/src/fpga/targets/pocket/core_top.v',
 '.integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gba_readonly.v',
 '.integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gba_s29gl256.v',
 '.integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gb_readonly.v',
 '.integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gb_readonly_abstract.v',
 '.integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gb_mbc5_fullrom.v',
 '.integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gb_mbc5_identify.v',
 '.integration/openfpgaos/src/firmware/os/kernel/services_table.c',
 '.integration/openfpgaos/src/firmware/os/targets/pocket/file.c',
 'app/gba_program65/program65.h','app/gba_program65/test_program65.c',
 'tests/build-ui98-fpga.ps1','tests/build-ui98-package.ps1','tests/build-ui98-os.ps1',
 'tests/check-ui98-host.sh','tests/check-ui98-rtl.sh','tests/check-ui98-periph.sh','tests/check-ui98-output.ps1')
$sources+=@(Get-ChildItem (Join-Path $root 'app/ui98') -File|ForEach-Object{'app/ui98/'+$_.Name})
$sources+=@(Get-ChildItem (Join-Path $root 'tests') -Filter 'test-ui98*' -File|ForEach-Object{'tests/'+$_.Name})
[IO.File]::WriteAllLines((Join-Path $bundle 'SOURCES_SHA256.txt'),[string[]]@($sources|ForEach-Object{"$(Sha (Join-Path $root $_))  $_"}),$utf8)
foreach($f in Get-ChildItem $sd -Recurse -Filter '*.json'){$null=Get-Content $f.FullName -Raw|ConvertFrom-Json}
foreach($slot in $data.data.data_slots){if($slot.filename -and !(Test-Path (Join-Path $common $slot.filename))){throw "Missing slot payload $($slot.filename)"}}
if(([int]$inputSlot.parameters -ne 3) -or $inputSlot.filename -or [bool]$inputSlot.nonvolatile){throw 'Slot4 must remain user-selected ROM input'}
if(([int]$outputSlot.parameters -ne 0) -or $outputSlot.filename -or [bool]$outputSlot.nonvolatile){throw 'Slot5 must remain dynamic backup output'}
$prefix=[IO.Path]::GetFullPath($sd).TrimEnd('\')+'\'
$sums=@(Get-ChildItem $sd -Recurse -File|Sort-Object FullName|ForEach-Object{"$(Sha $_.FullName)  $($_.FullName.Substring($prefix.Length).Replace('\','/'))"})
[IO.File]::WriteAllLines((Join-Path $bundle 'SHA256SUMS.txt'),[string[]]$sums,$utf8)
Compress-Archive -Path "$bundle/*" -DestinationPath "$bundle.zip" -CompressionLevel Optimal
Write-Output "smoke.101 package: $bundle.zip"
