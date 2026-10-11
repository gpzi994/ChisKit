[CmdletBinding()]
param(
 [string]$Distro='Ubuntu-24.04',
 [string]$OpenFpgaSdk='/home/lin13/work/ONS_FPGA/openfpgaSDK',
 [int]$Seed=5,
 [switch]$BridgeRepair,
 [double]$MinimumSetupSlackNs=0.30,
 [double]$MinimumHoldSlackNs=0.05
)
$ErrorActionPreference='Stop'
. (Join-Path $PSScriptRoot 'os-image-integrity.ps1')
function Sha([string]$p){(Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash.ToLowerInvariant()}
function Wsl([string]$cmd){& wsl.exe -d $Distro -- bash -lc $cmd;if($LASTEXITCODE -ne 0){throw "Build/test failed: $cmd"}}
function PrefixSha1([string]$p,[int]$n){
 $f=[IO.File]::OpenRead($p);try{$b=New-Object byte[] $n;if($f.Read($b,0,$n)-ne $n){throw 'Short ROM'}}finally{$f.Dispose()}
 $h=[Security.Cryptography.SHA1]::Create();try{[BitConverter]::ToString($h.ComputeHash($b)).Replace('-','').ToLowerInvariant()}finally{$h.Dispose()}
}
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$wroot='/mnt/'+$root.Substring(0,1).ToLowerInvariant()+'/'+$root.Substring(3).Replace('\','/')
$baseBundle=Join-Path $root 'build/smoke/ChisBurner-smoke.55-gba-db'
$base=Join-Path $baseBundle 'SD_ROOT'
$bundleLeaf=if($BridgeRepair){'ChisBurner-smoke.56b-gba-ident-bridge-fix'}else{'ChisBurner-smoke.56-gba-ident'}
$releaseVersion=if($BridgeRepair){'0.1.0-smoke.56b-gba-ident-bridge-fix'}else{'0.1.0-smoke.56-gba-ident'}
$bundle=Join-Path $root "build/smoke/$bundleLeaf"
$zip="$bundle.zip"
$build=Join-Path $root 'build/ident56-service'
$app=Join-Path $root 'build/ident56-app/app.elf'
$fpga=Join-Path $root ".integration/openfpgaos/src/fpga/targets/pocket/bld/chisburner-ident56-s$Seed-win"
$utf8=[Text.UTF8Encoding]::new($false)
if((Test-Path -LiteralPath $bundle) -or (Test-Path -LiteralPath $zip)){throw "Preserve existing package $bundleLeaf"}
if((Sha "$baseBundle.zip") -ne '07b5afd342c9950be1a2ef20e5f5883ee549a839176f41c168209efe247edb0f'){throw 'Wrong smoke55 base ZIP'}
foreach($line in Get-Content (Join-Path $baseBundle 'SHA256SUMS.txt')){
 if($line -notmatch '^([0-9a-f]{64})  (.+)$'){throw 'Bad base manifest'}
 if((Sha (Join-Path $base $Matches[2])) -ne $Matches[1]){throw "Changed smoke55 base $($Matches[2])"}
}

$rom='C:/Users/lin13/Downloads/[1MSRAM][FTC]口袋妖怪 - 宝石_中文典藏版[漫游&TGB联合汉化].gba'
$romHash='d1e41d5f8004de024391be96535bbc8952fb30a7b04b6e9c5a666d6927492dec'
if((Get-Item -LiteralPath $rom).Length -ne 33554432 -or (Sha $rom) -ne $romHash){throw 'Current Emerald image changed'}
$headerHash=PrefixSha1 $rom 384
if($headerHash -ne '63208478549c9b7063472e7e45cba83fed44ed73'){throw 'Unexpected BPEE header'}
$db=Get-Content -Raw -LiteralPath (Join-Path $root 'data/chisburner/db_AGB.json')|ConvertFrom-Json -AsHashtable
$emerald=$db[$headerHash];$wario=$db['e3497ec9cf54b086a7760c34c6207bc9c13707fd']
if(!$emerald -or $emerald.gc -ne 'BPEE' -or $emerald.rs -ne 16777216 -or $emerald.rc -ne 0x1f1c08fb){throw 'BPEE database contract changed'}
if(!$wario -or $wario.gc -ne 'AWAC' -or $wario.rs -ne 8388608 -or $wario.rc -ne 3480585713){throw 'AWAC database contract changed'}

Wsl "cd '$wroot/app/gba_ident56' && make host-test all OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0"
Wsl "cd '$wroot' && mkdir -p build/ident56-service && riscv64-unknown-elf-gcc -march=rv32imaf -mabi=ilp32f -Os -ffreestanding -fno-builtin -fno-pic -msmall-data-limit=0 -mno-relax -nostdlib -Wl,--no-relax,-T,tests/patches/identify56.ld -I.integration/openfpgaos/src/firmware/api .integration/openfpgaos/src/firmware/os/targets/pocket/identify56_services.c tests/patches/identify56_entry.S -o build/ident56-service/services.elf"
Wsl "cd '$wroot' && cc -DOF_PC -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined -I.integration/openfpgaos/src/firmware/api tests/test-ident56-services.c -o build/ident56-service/services-test && build/ident56-service/services-test"
Wsl "cd '$wroot' && rm -rf build/package-ident56-rtl && verilator --cc --exe --build -j 4 -Wno-fatal --top-module chisburner_cart_gba_readonly -Mdir build/package-ident56-rtl -GREADS_ENABLED=1 -GADDRESS_SETUP_CYCLES=12 -GSELECT_SETUP_CYCLES=6 -GTURNAROUND_CYCLES=3 -GDATA_WAIT_CYCLES=18 -GRELEASE_CYCLES=6 -GMAX_READ_COMMANDS=24 -GREQUIRED_READ_BYTES=32 -GREAD_ADDRESS_MASK=31 -GREAD_WINDOW_BYTES=384 -GIDENTIFY_ENABLED=1 -GMAX_IDENTIFY_COMMANDS=1 .integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gba_readonly.v '$wroot/tests/test-ident56-rtl.cpp' > build/ident56-service/rtl.log 2>&1 && build/package-ident56-rtl/Vchisburner_cart_gba_readonly"

$elf=[IO.File]::ReadAllBytes($app)
if($elf[0]-ne 0x7f -or $elf[4]-ne 1 -or $elf[5]-ne 1 -or [BitConverter]::ToUInt16($elf,18)-ne 243 -or
   ![Text.Encoding]::ASCII.GetString($elf).Contains('GBA IDENT smoke.56')){throw 'Wrong app ELF'}
$load=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && riscv64-unknown-elf-readelf -l build/ident56-app/app.elf") -join "`n"
if($load -notmatch 'LOAD\s+0x[0-9a-f]+\s+0x10400000\s+0x10400000' -or $load -match 'INTERP|DYNAMIC'){throw 'Unexpected app load map'}
$ui=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && build/ident56-app/pc-ui")
if($LASTEXITCODE -ne 0){throw 'UI fixture failed'}
$ui=$ui|ForEach-Object{[regex]::Replace([string]$_,'\x1b\[[0-9;]*[A-Za-z]','')}
$pages=($ui|Where-Object{$_ -match '^ GBA IDENT smoke\.56 [1-4]/4$'}).Count
$width=($ui|ForEach-Object{$_.Length}|Measure-Object -Maximum).Maximum
$rows=0;$maxRows=0;foreach($line in $ui){if($line -match '^ GBA IDENT smoke\.56'){$maxRows=[Math]::Max($maxRows,$rows);$rows=0};++$rows};$maxRows=[Math]::Max($maxRows,$rows)
if($pages-ne 4 -or $width-gt 40 -or $maxRows-gt 18){throw "UI shape $pages/$width/$maxRows"}

foreach($stage in @('map','fit','asm','mif_update')){
 if((Get-Content -Raw (Join-Path $fpga "output_files/ap_core.$stage.rpt")) -notmatch 'was successful\. 0 errors'){throw "FPGA stage failed $stage"}
}
$sta=Get-Content -Raw (Join-Path $fpga 'output_files/ap_core.sta.rpt');$slack=@{}
foreach($kind in @('setup','hold')){
 $m=[regex]::Matches($sta,"Worst-case $kind slack is\s+(-?\d+(?:\.\d+)?)");if(!$m.Count){throw "Missing $kind"}
 $slack[$kind]=($m|ForEach-Object{[double]$_.Groups[1].Value}|Measure-Object -Minimum).Minimum
 if($slack[$kind]-le 0){throw "Timing failed $kind $($slack[$kind])"}
}
if($BridgeRepair -and $slack['setup'] -lt $MinimumSetupSlackNs){throw "Bridge-repair setup margin too small: $($slack['setup']) ns < $MinimumSetupSlackNs ns"}
if($BridgeRepair -and $slack['hold'] -lt $MinimumHoldSlackNs){throw "Bridge-repair hold margin too small: $($slack['hold']) ns < $MinimumHoldSlackNs ns"}
$builtTop=Get-Content -Raw (Join-Path $fpga 'core_top_ident56.v')
if($builtTop -notmatch 'CHISBURNER_GBA_IDENT56' -or $builtTop -notmatch 'CHISBURNER_IDENTIFY_ENABLED\s*=\s*1'){throw 'Wrong FPGA profile'}
foreach($line in Get-Content (Join-Path $fpga 'SOURCE_SHA256.txt')){
 if($line -notmatch '^([0-9a-f]{64})  (.+)$' -or (Sha $Matches[2]) -ne $Matches[1]){throw "FPGA source drift $line"}
}

$serviceElf=Join-Path $build 'services.elf'
$nm=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && riscv64-unknown-elf-nm -n build/ident56-service/services.elf")
foreach($symbol in @('1032e200 T caps56','1032e340 T read56_entry','1032e3c0 T space56_entry','1032f180 T output_disabled56')){
 if($nm -cnotcontains $symbol){throw "Service symbol mismatch $symbol"}
}
$osRel='Assets/chisburner/common/os.bin';$old=[IO.File]::ReadAllBytes((Join-Path $base $osRel));$null=Assert-ChisBurnerOsIntegrity -Bytes $old;$os=[byte[]]$old.Clone()
$ranges=@(@('.caps56',0xE200,0x100),@('.entry56',0xE340,0x40),@('.space56',0xE3C0,0x40),@('.patch56',0xF180,0x280))
foreach($r in $ranges){
 Wsl "cd '$wroot' && riscv64-unknown-elf-objcopy -O binary -j $($r[0]) build/ident56-service/services.elf build/ident56-service/$($r[0]).bin"
 $b=[IO.File]::ReadAllBytes((Join-Path $build "$($r[0]).bin"));if(!$b.Length -or $b.Length-gt $r[2]){throw "Service region overflow $($r[0])"}
 for($i=$r[1];$i-lt $r[1]+$r[2];$i+=4){[Array]::Copy([byte[]](0x13,0,0,0),0,$os,$i,4)}
 [Array]::Copy($b,0,$os,$r[1],$b.Length)
}
$os=Set-ChisBurnerOsCrc -Bytes $os;$changed=0
for($i=0;$i-lt $old.Length-4;++$i){if($os[$i]-ne $old[$i]){$allowed=$false;foreach($r in $ranges){if($i-ge $r[1]-and $i-lt $r[1]+$r[2]){$allowed=$true}};if(!$allowed){throw 'OS write outside allowlist'};++$changed}}
$osCrc=Assert-ChisBurnerOsIntegrity -Bytes $os

$runtime=@('app/gba_ident56/main.c','app/gba_ident56/identify56.c','app/gba_ident56/identify56.h','app/gba_ident56/db55_link.c','app/gba_ident56/db55_data_link.c','app/gba_db55/db55.c','app/gba_db55/db55.h','app/gba_db55/db55_data.c')
foreach($p in $runtime){$s=Get-Content -Raw -LiteralPath (Join-Path $root $p);if($s-match 'of_(output|file_write)|of_cart_(write|erase|program)|OF_CART_SPACE_(SAVE|RTC)'){throw "Mutation call in $p"}}

$sd=Join-Path $bundle 'SD_ROOT';New-Item -ItemType Directory -Path $bundle|Out-Null
$omit=@('Assets/chisburner/common/chisburner55.elf','Assets/chisburner/common/chisburner55.ini')
$prefix=[IO.Path]::GetFullPath($base).TrimEnd('\')+'\'
foreach($f in Get-ChildItem -LiteralPath $base -Recurse -File){
 $rel=$f.FullName.Substring($prefix.Length).Replace('\','/');if($rel-in $omit){continue}
 $dest=Join-Path $sd $rel;New-Item -ItemType Directory -Force (Split-Path $dest -Parent)|Out-Null;Copy-Item -LiteralPath $f.FullName -Destination $dest
}
[IO.File]::WriteAllBytes((Join-Path $sd $osRel),$os)
$common=Join-Path $sd 'Assets/chisburner/common';Copy-Item -LiteralPath $app -Destination (Join-Path $common 'chisburner56.elf')
[IO.File]::WriteAllText((Join-Path $common 'chisburner56.ini'),"[os]`nELF=chisburner56.elf`nARGS=`n",$utf8)
$rbfRel='Cores/ChisBurner.ChisBurner/chis.rbf_r'
if(-not ('ChisBurner.Ident56.Reverse' -as [type])){Add-Type -TypeDefinition @'
namespace ChisBurner.Ident56 {
 public static class Reverse {
  public static byte[] Apply(byte[] a) {
   var t = new byte[256];
   for (int i=0;i<256;i++) { int x=i,y=0; for(int j=0;j<8;j++){y=(y<<1)|(x&1);x>>=1;} t[i]=(byte)y; }
   var b = new byte[a.Length]; for(int i=0;i<a.Length;i++) b[i]=t[a[i]]; return b;
  }
 }
}
'@}
$rawRbf=[IO.File]::ReadAllBytes((Join-Path $fpga 'output_files/ap_core.rbf'))
$rbfR=[ChisBurner.Ident56.Reverse]::Apply($rawRbf)
$roundtrip=[ChisBurner.Ident56.Reverse]::Apply($rbfR)
if([BitConverter]::ToString([ChisBurner.Ident56.Reverse]::Apply([byte[]](1,128,18,170))) -ne '80-01-48-55'){throw 'RBF bit-reversal vector failed'}
if($rawRbf.Length -ne $roundtrip.Length){throw 'RBF bit-reversal length changed'}
for($i=0;$i-lt $rawRbf.Length;++$i){if($rawRbf[$i]-ne $roundtrip[$i]){throw "RBF bit-reversal round-trip failed at $i"}}
[IO.File]::WriteAllBytes((Join-Path $sd $rbfRel),$rbfR)
$corePath=Join-Path $sd 'Cores/ChisBurner.ChisBurner/core.json';$core=Get-Content -Raw $corePath|ConvertFrom-Json
$core.core.framework.hardware.cartridge_adapter=16777216;$core.core.metadata.version=$releaseVersion;$core.core.metadata.date_release='2026-09-04'
[IO.File]::WriteAllText($corePath,($core|ConvertTo-Json -Depth 25),$utf8)
$dataPath=Join-Path $sd 'Cores/ChisBurner.ChisBurner/data.json';$data=Get-Content -Raw $dataPath|ConvertFrom-Json
foreach($slot in $data.data.data_slots){switch([int]$slot.id){2{$slot.filename='chisburner56.ini'}3{$slot.filename='chisburner56.elf'}4{$slot.name='Unused';$slot.parameters=0;$slot.PSObject.Properties.Remove('filename');$slot.PSObject.Properties.Remove('size_exact')}}}
[IO.File]::WriteAllText($dataPath,($data|ConvertTo-Json -Depth 25),$utf8)
foreach($json in Get-ChildItem -LiteralPath $sd -Filter '*.json' -Recurse -File){$null=Get-Content -Raw $json.FullName|ConvertFrom-Json}
if(Test-Path (Join-Path $sd 'Saves')){throw 'No Saves directory allowed'}
Copy-Item -LiteralPath (Join-Path $root 'app/gba_ident56/README_TEST.txt') -Destination $bundle
Copy-Item -LiteralPath (Join-Path $fpga 'SOURCE_SHA256.txt') -Destination (Join-Path $bundle 'FPGA_SOURCE_SHA256.txt')
[IO.File]::WriteAllLines((Join-Path $bundle 'UI_FIXTURES_SYNTHETIC.txt'),[string[]]$ui,$utf8)
$sources=@($runtime+@('app/gba_ident56/test_ident56.c','app/gba_ident56/Makefile','app/gba_ident56/README_TEST.txt','tests/build-ident56.ps1','tests/build-ident56-fpga.ps1','tests/test-ident56-rtl.cpp','tests/test-ident56-services.c','tests/patches/identify56.ld','tests/patches/identify56_entry.S','.integration/openfpgaos/src/firmware/os/targets/pocket/identify56_services.c','.integration/openfpgaos/src/firmware/api/of_cart.h','.integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gba_readonly.v','core/cartridge_hal/identify56_policy.vh'))|Select-Object -Unique
[IO.File]::WriteAllLines((Join-Path $bundle 'SOFTWARE_SOURCE_SHA256.txt'),[string[]]@($sources|ForEach-Object{"$(Sha (Join-Path $root $_))  $_"}),$utf8)
$proof=@"
smoke=56-gba-cartridge-identification
package_revision=$releaseVersion
bridge_repair=$BridgeRepair
excluded_package=smoke.56 seed6;physical failure before app: Error in framework / RS bridge not responding
bridge_failure_root_cause=smoke.56 packaged raw Quartus RBF without required per-byte bit reversal
hardware_result=PENDING;not yet tested on Pocket
scope=content header database plus fixed NOR ID/CFI;no dump/save/TF write/program/erase
database_entries=2923
current_file=[1MSRAM][FTC] Chinese Pokemon Emerald
current_file_size=33554432
current_file_sha256=$romHash
current_header_sha1=$headerHash
current_content=BPEE;DB ROM 16777216;DB CRC32 1F1C08FB;save type 5;save 131072
current_expected_hardware=S29GL256N compatible;CFI exponent 25;physical 33554432
current_final_dump_plan=33554432 physical capacity overrides DB content size
wario_content=AWAC;DB ROM 8388608;DB CRC32 CF7585F1;save type 3;save 32768
wario_expected_hardware=generic ROM path;no supported NOR;not authenticity certificate
wario_final_dump_plan=8388608
probe=F0,baseline,555-AA,2AA-55,555-90,ID,F0,55-98,QRY,size,F0
probe_result_bytes=baseline8,autoselect8,cfi_qry8,cfi_size8
accepted_profile=manufacturer 0001,device 227E,QRY,exponent25
generic_write_address_or_data_register=ABSENT
program_erase_opcodes=A0,80,10,30,25,29 ABSENT;verified by RTL event trace
writes_enabled=0
FPGA_identify_one_shot_and_generic_regression=PASS Verilator
OS_caps_ROM_identify_shape_and_reject_without_MMIO=PASS ASan UBSan
SDK_app_and_2923_entry_parser=PASS
FPGA_seed=$Seed
FPGA_setup_slack_ns=$($slack['setup'])
FPGA_hold_slack_ns=$($slack['hold'])
FPGA_required_setup_slack_ns=$(if($BridgeRepair){$MinimumSetupSlackNs}else{0})
FPGA_required_hold_slack_ns=$(if($BridgeRepair){$MinimumHoldSlackNs}else{0})
FPGA_compile_map_fit_asm_mif_update=PASS
FPGA_failed_seed4_setup_slack_ns=-0.007;not packaged
FPGA_failed_seed5_setup_slack_ns=0.081;hold_slack_ns=-0.015;not packaged
OS_base=smoke55;fixed layout allowlist E200..E2FF,E340..E37F,E3C0..E3FF,F180..F3FF
OS_changed_bytes_excluding_crc=$changed
OS_internal_crc=$('{0:x8}' -f $osCrc.ComputedCrc)
OS_sha256=$(Sha (Join-Path $sd $osRel))
RBF_sha256=$(Sha (Join-Path $sd $rbfRel))
RBF_raw_sha256=$(Sha (Join-Path $fpga 'output_files/ap_core.rbf'))
RBF_packaging=per-byte bit reversed and exact double-reversal round-trip verified
app_sha256=$(Sha (Join-Path $common 'chisburner56.elf'))
UI_pages=$pages;max_columns=$width;max_rows=$maxRows
physical_test=each cartridge separately;fresh core;no hot swap;page1 then A once then pages3-4
"@
[IO.File]::WriteAllText((Join-Path $bundle 'BUILD_PROVENANCE.txt'),$proof,$utf8)
$sdPrefix=[IO.Path]::GetFullPath($sd).TrimEnd('\')+'\';$hashes=@{}
$sums=Get-ChildItem -LiteralPath $sd -Recurse -File|Sort-Object FullName|ForEach-Object{$rel=$_.FullName.Substring($sdPrefix.Length).Replace('\','/');$h=Sha $_.FullName;$hashes["SD_ROOT/$rel"]=$h;"$h  $rel"}
[IO.File]::WriteAllLines((Join-Path $bundle 'SHA256SUMS.txt'),[string[]]$sums,$utf8)
Compress-Archive -Path (Join-Path $bundle '*') -DestinationPath $zip -CompressionLevel Optimal
$null=Assert-ChisBurnerZipOsIntegrity -ZipPath $zip
Add-Type -AssemblyName System.IO.Compression.FileSystem;$archive=[IO.Compression.ZipFile]::OpenRead($zip)
try{foreach($name in $hashes.Keys){$e=$archive.GetEntry($name);if(!$e){throw "Missing ZIP payload $name"};$s=$e.Open();$h=[Security.Cryptography.SHA256]::Create();try{$actual=[BitConverter]::ToString($h.ComputeHash($s)).Replace('-','').ToLowerInvariant();if($actual-ne $hashes[$name]){throw "ZIP mismatch $name"}}finally{$s.Dispose();$h.Dispose()}}}finally{$archive.Dispose()}
if((Sha $rom)-ne $romHash){throw 'Current ROM modified during build'}
"PASS: $zip"
"ZIP SHA256: $(Sha $zip)"
"Seed $Seed timing setup=$($slack['setup']) hold=$($slack['hold']); $($hashes.Count) ZIP payloads verified"
