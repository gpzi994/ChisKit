[CmdletBinding()]
param(
 [string]$Distro='Ubuntu-24.04',
 [string]$OpenFpgaSdk='/home/lin13/work/ONS_FPGA/openfpgaSDK',
 [int]$Seed=7
)
$ErrorActionPreference='Stop'
. (Join-Path $PSScriptRoot 'os-image-integrity.ps1')
function Sha([string]$p){(Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash.ToLowerInvariant()}
function Wsl([string]$cmd){& wsl.exe -d $Distro -- bash -lc $cmd;if($LASTEXITCODE-ne 0){throw "Build/test failed: $cmd"}}
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$wroot='/mnt/'+$root.Substring(0,1).ToLowerInvariant()+'/'+$root.Substring(3).Replace('\','/')
$baseBundle=Join-Path $root 'build/smoke/ChisBurner-smoke.56b-gba-ident-bridge-fix'
$base=Join-Path $baseBundle 'SD_ROOT'
$bundle=Join-Path $root 'build/smoke/ChisBurner-smoke.57-emerald32-dump'
$zip="$bundle.zip"
$build=Join-Path $root 'build/dump57-service'
$app=Join-Path $root 'build/dump57-app/app.elf'
$fpga=Join-Path $root ".integration/openfpgaos/src/fpga/targets/pocket/bld/chisburner-dump57-s$Seed-win"
$utf8=[Text.UTF8Encoding]::new($false)
if((Test-Path -LiteralPath $bundle) -or (Test-Path -LiteralPath $zip)){throw 'Preserve existing smoke57 package'}
if((Sha "$baseBundle.zip") -ne 'f15f44e40a0fae6fd78b15487f52c767e0a0f3a1757cea12e985c6f2bd2e783c'){throw 'Wrong smoke56b base ZIP'}
foreach($line in Get-Content (Join-Path $baseBundle 'SHA256SUMS.txt')){
 if($line -notmatch '^([0-9a-f]{64})  (.+)$'){throw 'Bad base manifest'}
 if((Sha (Join-Path $base $Matches[2])) -ne $Matches[1]){throw "Changed smoke56b base $($Matches[2])"}
}

$rom='C:/Users/lin13/Downloads/[1MSRAM][FTC]口袋妖怪 - 宝石_中文典藏版[漫游&TGB联合汉化].gba'
$romSha='d1e41d5f8004de024391be96535bbc8952fb30a7b04b6e9c5a666d6927492dec'
if((Get-Item -LiteralPath $rom).Length -ne 33554432 -or (Sha $rom) -ne $romSha){throw 'Current Emerald source changed'}
if((Get-FileHash -LiteralPath $rom -Algorithm MD5).Hash -ne '872A1A05E39006F3BEA69EFAA641B3A4'){throw 'Current Emerald MD5 changed'}

Wsl "cd '$wroot/app/gba_dump57' && make host-test all OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0"
Wsl "cd '$wroot' && mkdir -p build/dump57-service && riscv64-unknown-elf-gcc -march=rv32imafc -mabi=ilp32f -Os -ffreestanding -fno-builtin -fno-pic -msmall-data-limit=0 -mno-relax -nostdlib -Wl,--no-relax,-T,tests/patches/dump57.ld -I.integration/openfpgaos/src/firmware/api .integration/openfpgaos/src/firmware/os/targets/pocket/dump57_services.c tests/patches/dump57_entry.S -o build/dump57-service/services.elf"
Wsl "cd '$wroot' && cc -DOF_PC -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined -I.integration/openfpgaos/src/firmware/api tests/test-dump57-services.c -o build/dump57-service/services-test && build/dump57-service/services-test"
$relocs=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && riscv64-unknown-elf-readelf -r build/dump57-service/services.elf") -join "`n"
if($LASTEXITCODE-ne 0 -or $relocs -notmatch 'There are no relocations'){throw 'Service has relocations'}
$nm=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && riscv64-unknown-elf-nm -n build/dump57-service/services.elf")
foreach($symbol in @('1032e200 T caps57','1032e340 T read57_entry','1032e3c0 T space57_entry','1032f180 T bind57','1032f21a T xfer57','1032f362 T ident57')){
 if($nm -cnotcontains $symbol){throw "Service symbol mismatch $symbol"}
}
Wsl "cd '$wroot' && rm -rf build/package-dump57-rtl && verilator --cc --exe --build -j 4 -Wno-fatal --top-module chisburner_cart_gba_readonly -Mdir build/package-dump57-rtl -GREADS_ENABLED=1 -GADDRESS_SETUP_CYCLES=12 -GSELECT_SETUP_CYCLES=6 -GTURNAROUND_CYCLES=3 -GDATA_WAIT_CYCLES=18 -GRELEASE_CYCLES=6 -GMAX_READ_COMMANDS=1048605 -GREQUIRED_READ_BYTES=32 -GREAD_ADDRESS_MASK=31 -GREAD_WINDOW_BYTES=33554432 -GIDENTIFY_ENABLED=1 -GMAX_IDENTIFY_COMMANDS=1 .integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gba_readonly.v '$wroot/tests/test-dump57-rtl.cpp' > build/dump57-service/rtl.log 2>&1 && build/package-dump57-rtl/Vchisburner_cart_gba_readonly"
Wsl "cd '$wroot' && rm -rf build/package-dump57-ident-rtl && verilator --cc --exe --build -j 4 -Wno-fatal --top-module chisburner_cart_gba_readonly -Mdir build/package-dump57-ident-rtl -GREADS_ENABLED=1 -GADDRESS_SETUP_CYCLES=12 -GSELECT_SETUP_CYCLES=6 -GTURNAROUND_CYCLES=3 -GDATA_WAIT_CYCLES=18 -GRELEASE_CYCLES=6 -GMAX_READ_COMMANDS=1048605 -GREQUIRED_READ_BYTES=32 -GREAD_ADDRESS_MASK=31 -GREAD_WINDOW_BYTES=33554432 -GIDENTIFY_ENABLED=1 -GMAX_IDENTIFY_COMMANDS=1 .integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gba_readonly.v '$wroot/tests/test-ident56-rtl.cpp' > build/dump57-service/ident-rtl.log 2>&1 && build/package-dump57-ident-rtl/Vchisburner_cart_gba_readonly"

$elf=[IO.File]::ReadAllBytes($app)
if($elf[0]-ne 0x7f -or $elf[4]-ne 1 -or $elf[5]-ne 1 -or [BitConverter]::ToUInt16($elf,18)-ne 243 -or
   ![Text.Encoding]::ASCII.GetString($elf).Contains('GBA DUMP smoke.57')){throw 'Wrong app ELF'}
$load=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && riscv64-unknown-elf-readelf -l build/dump57-app/app.elf") -join "`n"
if($load -notmatch 'LOAD\s+0x[0-9a-f]+\s+0x10400000\s+0x10400000' -or $load -match 'INTERP|DYNAMIC'){throw 'Unexpected app load map'}
$ui=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && build/dump57-app/pc-ui")
if($LASTEXITCODE-ne 0){throw 'UI fixture failed'}
$ui=$ui|ForEach-Object{[regex]::Replace([string]$_,'\x1b\[[0-9;]*[A-Za-z]','')}
$width=($ui|ForEach-Object{$_.Length}|Measure-Object -Maximum).Maximum
$titles=($ui|Where-Object{$_ -eq ' GBA DUMP smoke.57'}).Count
$rows=0;$maxRows=0;foreach($line in $ui){if($line-eq ' GBA DUMP smoke.57'){$maxRows=[Math]::Max($maxRows,$rows);$rows=0};++$rows};$maxRows=[Math]::Max($maxRows,$rows)
if($width-gt 40 -or $maxRows-gt 18 -or $titles-ne 12){throw "UI shape $titles/$width/$maxRows"}

foreach($line in Get-Content (Join-Path $fpga 'SOURCE_SHA256.txt')){
 if($line -notmatch '^([0-9a-f]{64})  (.+)$' -or (Sha $Matches[2]) -ne $Matches[1]){throw "FPGA source drift $line"}
}
$qsf=Get-Content -Raw (Join-Path $fpga 'ap_core.qsf')
foreach($macro in @('INCLUDE_CARTRIDGE','CHISBURNER_GBA_IDENT56')){
 if($qsf -notmatch "(?m)^set_global_assignment -name VERILOG_MACRO $macro\s*$"){throw "Missing FPGA macro $macro"}
}
$policy=Get-Content -Raw (Join-Path $root 'core/cartridge_hal/dump57_policy.vh')
$builtTop=Get-Content -Raw (Join-Path $fpga 'core_top_dump57.v')
if(!$builtTop.Contains($policy) -or $builtTop -notmatch 'CHISBURNER_IDENTIFY_ENABLED\s*=\s*1'){throw 'Wrong built FPGA policy'}
foreach($stage in @('map','fit','asm','mif_update')){
 if((Get-Content -Raw (Join-Path $fpga "output_files/ap_core.$stage.rpt")) -notmatch 'was successful\. 0 errors'){throw "FPGA stage failed $stage"}
}
$mif=Get-Content -Raw (Join-Path $fpga 'output_files/ap_core.mif_update.rpt')
if($mif -notmatch 'Processed Memory Initialization File .*firmware\.mif' -or
   $mif -notmatch 'Processed Memory Initialization File .*build_id\.mif' -or
   $mif -match 'Critical Warning|can.t find|setting all initial values to 0'){throw 'Boot MIF not verified'}
$sta=Get-Content -Raw (Join-Path $fpga 'output_files/ap_core.sta.rpt');$slack=@{}
foreach($kind in @('setup','hold')){
 $m=[regex]::Matches($sta,"Worst-case $kind slack is\s+(-?\d+(?:\.\d+)?)");if(!$m.Count){throw "Missing $kind"}
 $slack[$kind]=($m|ForEach-Object{[double]$_.Groups[1].Value}|Measure-Object -Minimum).Minimum
 if($slack[$kind]-le 0){throw "Timing failed $kind $($slack[$kind])"}
}

$osRel='Assets/chisburner/common/os.bin';$old=[IO.File]::ReadAllBytes((Join-Path $base $osRel))
$null=Assert-ChisBurnerOsIntegrity -Bytes $old;$os=[byte[]]$old.Clone()
$ranges=@(@('.caps57',0xE200,0x100),@('.entry57',0xE340,0x40),@('.space57',0xE3C0,0x40),@('.patch57',0xF180,0x280))
foreach($r in $ranges){
 Wsl "cd '$wroot' && riscv64-unknown-elf-objcopy -O binary -j $($r[0]) build/dump57-service/services.elf build/dump57-service/$($r[0]).bin"
 $b=[IO.File]::ReadAllBytes((Join-Path $build "$($r[0]).bin"));if(!$b.Length -or $b.Length-gt $r[2]){throw "Service overflow $($r[0])"}
 for($i=$r[1];$i-lt $r[1]+$r[2];$i+=4){[Array]::Copy([byte[]](0x13,0,0,0),0,$os,$i,4)}
 [Array]::Copy($b,0,$os,$r[1],$b.Length)
}
$os=Set-ChisBurnerOsCrc -Bytes $os;$changed=0
for($i=0;$i-lt $old.Length-4;++$i){if($os[$i]-ne $old[$i]){
 $allowed=$false;foreach($r in $ranges){if($i-ge $r[1] -and $i-lt $r[1]+$r[2]){$allowed=$true}}
 if(!$allowed){throw "OS write outside allowlist at $i"};++$changed
}}
if($os.Length-ne 137480){throw 'OS layout changed'};$osCrc=Assert-ChisBurnerOsIntegrity -Bytes $os

$runtime=@('app/gba_dump57/main.c','app/gba_dump57/dump57.h','app/gba_dump57/read_block.h','app/gba_dump57/reference.h','app/gba_dump57/identify56_link.c','app/gba_dump57/db55_link.c','app/gba_ident56/identify56.c','app/gba_ident56/identify56.h','app/gba_db55/db55.c','app/gba_db55/db55.h','app/gba_db55/db55_data.c','app/gba_dump53/md5.h','app/storage_stream52/gate.h')
foreach($p in $runtime){
 $s=Get-Content -Raw -LiteralPath (Join-Path $root $p)
 if($s-match '\bof_output_flush\s*\(|\bof_cart_(write|erase|program)\s*\(|OF_CART_SPACE_(SAVE|RTC)'){throw "Mutation path in $p"}
}

$sd=Join-Path $bundle 'SD_ROOT';New-Item -ItemType Directory -Path $bundle|Out-Null
$omit=@('Assets/chisburner/common/chisburner56.elf','Assets/chisburner/common/chisburner56.ini')
$basePrefix=[IO.Path]::GetFullPath($base).TrimEnd('\')+'\'
foreach($f in Get-ChildItem -LiteralPath $base -Recurse -File){
 $rel=$f.FullName.Substring($basePrefix.Length).Replace('\','/');if($rel-in $omit){continue}
 $dest=Join-Path $sd $rel;New-Item -ItemType Directory -Force (Split-Path $dest -Parent)|Out-Null;Copy-Item -LiteralPath $f.FullName -Destination $dest
}
[IO.File]::WriteAllBytes((Join-Path $sd $osRel),$os)
$common=Join-Path $sd 'Assets/chisburner/common';Copy-Item -LiteralPath $app -Destination (Join-Path $common 'chisburner57.elf')
[IO.File]::WriteAllText((Join-Path $common 'chisburner57.ini'),"[os]`nELF=chisburner57.elf`nARGS=`n",$utf8)
$coreRel='Cores/ChisBurner.ChisBurner/core.json';$corePath=Join-Path $sd $coreRel
$core=Get-Content -Raw $corePath|ConvertFrom-Json
$core.core.framework.hardware.cartridge_adapter=16777216;$core.core.metadata.version='0.1.0-smoke.57-emerald32-dump';$core.core.metadata.date_release='2026-09-04'
[IO.File]::WriteAllText($corePath,($core|ConvertTo-Json -Depth 25),$utf8)
$dataRel='Cores/ChisBurner.ChisBurner/data.json';$dataPath=Join-Path $sd $dataRel
$data=Get-Content -Raw $dataPath|ConvertFrom-Json
foreach($slot in $data.data.data_slots){switch([int]$slot.id){
 2{$slot.filename='chisburner57.ini'}
 3{$slot.filename='chisburner57.elf'}
 4{$slot.name='GBA Dump 57 32MiB';$slot|Add-Member -NotePropertyName filename -NotePropertyValue 'dump57-32m.gba' -Force;$slot.extensions=@('gba');$slot.parameters=2;$slot.deferload=$true;$slot.required=$false;$slot.PSObject.Properties.Remove('nonvolatile');$slot.PSObject.Properties.Remove('address');$slot.PSObject.Properties.Remove('size_maximum');$slot|Add-Member -NotePropertyName size_exact -NotePropertyValue 33554432 -Force}
}}
[IO.File]::WriteAllText($dataPath,($data|ConvertTo-Json -Depth 25),$utf8)
$fixtureRel='Assets/chisburner/ChisBurner.ChisBurner/dump57-32m.gba';$fixture=Join-Path $sd $fixtureRel
New-Item -ItemType Directory -Force (Split-Path $fixture -Parent)|Out-Null
$stream=[IO.File]::Create($fixture);try{$stream.SetLength(33554432)}finally{$stream.Dispose()}
if((Get-FileHash -LiteralPath $fixture -Algorithm MD5).Hash-ne '58F06DD588D8FFB3BEB46ADA6309436B' -or
   (Sha $fixture)-ne '83ee47245398adee79bd9c0a8bc57b821e92aba10f5f9ade8a5d1fae4d8c4302'){throw 'Wrong zero fixture'}

$rbfRel='Cores/ChisBurner.ChisBurner/chis.rbf_r'
if(-not ('ChisBurner.Dump57.Reverse' -as [type])){Add-Type -TypeDefinition @'
namespace ChisBurner.Dump57 { public static class Reverse { public static byte[] Apply(byte[] a) {
 var t=new byte[256]; for(int i=0;i<256;i++){int x=i,y=0;for(int j=0;j<8;j++){y=(y<<1)|(x&1);x>>=1;}t[i]=(byte)y;}
 var b=new byte[a.Length];for(int i=0;i<a.Length;i++)b[i]=t[a[i]];return b;
} } }
'@}
$rawRbf=[IO.File]::ReadAllBytes((Join-Path $fpga 'output_files/ap_core.rbf'))
$rbfR=[ChisBurner.Dump57.Reverse]::Apply($rawRbf);$round=[ChisBurner.Dump57.Reverse]::Apply($rbfR)
if([BitConverter]::ToString([ChisBurner.Dump57.Reverse]::Apply([byte[]](1,128,18,170)))-ne '80-01-48-55'){throw 'RBF reversal vector failed'}
if($rawRbf.Length-ne $round.Length){throw 'RBF reversal length changed'}
for($i=0;$i-lt $rawRbf.Length;++$i){if($rawRbf[$i]-ne $round[$i]){throw "RBF reversal roundtrip failed $i"}}
[IO.File]::WriteAllBytes((Join-Path $sd $rbfRel),$rbfR)

foreach($json in Get-ChildItem -LiteralPath $sd -Filter '*.json' -Recurse -File){$null=Get-Content -Raw $json.FullName|ConvertFrom-Json}
foreach($id in 1,2,3,4){
 $slot=$data.data.data_slots|Where-Object id -eq $id;$dir=if($id -eq 4){'Assets/chisburner/ChisBurner.ChisBurner'}else{'Assets/chisburner/common'}
 $file=Join-Path (Join-Path $sd $dir) $slot.filename;if(!(Test-Path -LiteralPath $file)){throw "Missing slot $id file"}
}
if((Get-Item -LiteralPath $fixture).Length-ne 33554432){throw 'Slot4 size mismatch'}
if(Test-Path (Join-Path $sd 'Saves')){throw 'No Saves directory allowed'}
Copy-Item -LiteralPath (Join-Path $root 'app/gba_dump57/README_TEST.txt') -Destination $bundle
Copy-Item -LiteralPath (Join-Path $root 'tests/Compare-ROM57.ps1') -Destination $bundle
Copy-Item -LiteralPath (Join-Path $fpga 'SOURCE_SHA256.txt') -Destination (Join-Path $bundle 'FPGA_SOURCE_SHA256.txt')
[IO.File]::WriteAllLines((Join-Path $bundle 'UI_FIXTURES_SYNTHETIC.txt'),[string[]]$ui,$utf8)
$sources=@($runtime+@('app/gba_dump57/test_dump57.c','app/gba_dump57/Makefile','app/gba_dump57/README_TEST.txt','tests/Compare-ROM57.ps1','tests/build-dump57.ps1','tests/build-dump57-fpga.ps1','tests/test-dump57-rtl.cpp','tests/test-ident56-rtl.cpp','tests/test-dump57-services.c','tests/patches/dump57.ld','tests/patches/dump57_entry.S','tests/os-image-integrity.ps1','.integration/openfpgaos/src/firmware/os/targets/pocket/dump57_services.c','.integration/openfpgaos/src/firmware/api/of_cart.h','.integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gba_readonly.v','core/cartridge_hal/dump57_policy.vh'))|Select-Object -Unique
[IO.File]::WriteAllLines((Join-Path $bundle 'SOFTWARE_SOURCE_SHA256.txt'),[string[]]@($sources|ForEach-Object{"$(Sha (Join-Path $root $_))  $_"}),$utf8)
$proof=@"
smoke=57-emerald32-identify-and-dump
hardware_result=PENDING;this package has not yet run on Pocket
verified_physical_base=smoke56b RBF bit-reversal repair;BPEE S29GL256N and AWAC generic branches passed user testing
scope=current exact 33554432-byte BPEE translation on 32MiB ChisFlash only
source_sha256=$romSha
source_md5=872a1a05e39006f3bea69efaa641b3a4
source_full_crc32=d81fc708
source_first16MiB_crc32=a03dc885
database_canonical_BPEE_crc32=1f1c08fb;identification reference only, not patched ROM payload CRC
identification=two 384-byte headers, database BPEE 16MiB, fixed one-shot NOR ID/CFI, require physical32MiB
wario_AWAC_expected=database8MiB,generic ROM,no supported NOR,stop before TF write
static_output=dump57-32m.gba;33554432 zero bytes;1024x32768
runtime=boot full TF scan;guard;five samples;cart32x1048576;TF32KiBx1024+immediate readback;cart hashes;independent full TF verify
cart_normal_read_budget=1048605 (24 header + 5 samples + 1048576 full pass)
identify_budget=1 fixed FPGA sequence
cart_write_save_rtc_erase_voltage_switch=structurally unavailable
program_erase_opcodes=A0,80,10,30,25,29 absent from fixed identify sequence;RTL checked
flush=never called;previous RC -12 not hidden or converted
SDK_app_full_source_and_fault_gates=PASS ASan UBSan
OS_service_caps_ROM_IDENTIFY_static_output_and_rejects=PASS ASan UBSan
OS_service_elf_relocations=NONE
FPGA_full_read_budget_boundary_safe_pins=PASS Verilator
FPGA_identify_supported_generic_one_shot_forbidden_opcodes=PASS Verilator
FPGA_seed=$Seed
FPGA_setup_slack_ns=$($slack['setup'])
FPGA_hold_slack_ns=$($slack['hold'])
FPGA_map_fit_asm_mif_update=PASS
OS_base=physically accepted smoke56b;fixed allowlist E200..E2FF,E340..E37F,E3C0..E3FF,F180..F3FF
OS_changed_bytes_excluding_crc=$changed
OS_internal_crc=$('{0:x8}' -f $osCrc.ComputedCrc)
OS_sha256=$(Sha (Join-Path $sd $osRel))
RBF_raw_sha256=$(Sha (Join-Path $fpga 'output_files/ap_core.rbf'))
RBF_packaged_sha256=$(Sha (Join-Path $sd $rbfRel))
RBF_packaging=per-byte bit reversed and exact double-reversal roundtrip verified
app_sha256=$(Sha (Join-Path $common 'chisburner57.elf'))
UI_titles=$titles;max_columns=$width;max_rows=$maxRows
official_data_slot=size_exact 33554432,deferload true,parameters2 static output
physical_test=copy whole SD_ROOT,current BPEE ChisFlash only,first zero seed dump,then normal quit/power cycle ROM VERIFIED
"@
[IO.File]::WriteAllText((Join-Path $bundle 'BUILD_PROVENANCE.txt'),$proof,$utf8)
if((Sha $rom)-ne $romSha){throw 'Reference modified during build'}
$sdPrefix=[IO.Path]::GetFullPath($sd).TrimEnd('\')+'\';$hashes=@{}
$sums=Get-ChildItem -LiteralPath $sd -Recurse -File|Sort-Object FullName|ForEach-Object{
 $rel=$_.FullName.Substring($sdPrefix.Length).Replace('\','/');$h=Sha $_.FullName;$hashes["SD_ROOT/$rel"]=$h;"$h  $rel"
}
if($hashes.Count-ne 16){throw "Unexpected payload count $($hashes.Count)"}
[IO.File]::WriteAllLines((Join-Path $bundle 'SHA256SUMS.txt'),[string[]]$sums,$utf8)
Compress-Archive -Path (Join-Path $bundle '*') -DestinationPath $zip -CompressionLevel Optimal
$null=Assert-ChisBurnerZipOsIntegrity -ZipPath $zip
Add-Type -AssemblyName System.IO.Compression.FileSystem;$archive=[IO.Compression.ZipFile]::OpenRead($zip)
try{foreach($name in $hashes.Keys){
 $entry=$archive.GetEntry($name);if(!$entry){throw "Missing ZIP payload $name"}
 $stream=$entry.Open();$hash=[Security.Cryptography.SHA256]::Create();try{$actual=[BitConverter]::ToString($hash.ComputeHash($stream)).Replace('-','').ToLowerInvariant()}finally{$stream.Dispose();$hash.Dispose()}
 if($actual-ne $hashes[$name]){throw "ZIP payload hash mismatch $name"}
}}finally{$archive.Dispose()}
"PASS: $zip"
"ZIP SHA256: $(Sha $zip)"
"OS allowlist + CRC, RBF bit reversal, exact slot4, all $($hashes.Count) ZIP payload hashes verified"
