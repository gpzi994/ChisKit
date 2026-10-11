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
$baseBundle=Join-Path $root 'build/smoke/ChisBurner-smoke.58-dynamic-create'
$base=Join-Path $baseBundle 'SD_ROOT'
$bundle=Join-Path $root 'build/smoke/ChisBurner-smoke.59-open-param-trace-final'
$zip="$bundle.zip"
$app=Join-Path $root 'build/storage-dynamic59-app/app.elf'
$fpga=Join-Path $root ".integration/openfpgaos/src/fpga/targets/pocket/bld/chisburner-dynamic59-s$Seed-win"
$utf8=[Text.UTF8Encoding]::new($false)
if((Test-Path -LiteralPath $bundle) -or (Test-Path -LiteralPath $zip)){throw 'Preserve existing smoke59 package'}
if((Sha "$baseBundle.zip")-ne 'f05c97e91f04b7253540b709217433de3e6e749f188c6bf7878dcc186fe0bf08'){throw 'Wrong smoke58 base ZIP'}
foreach($line in Get-Content (Join-Path $baseBundle 'SHA256SUMS.txt')){
 if($line-notmatch '^([0-9a-f]{64})  (.+)$'){throw 'Bad base manifest'}
 if((Sha (Join-Path $base $Matches[2]))-ne $Matches[1]){throw "Changed smoke58 base $($Matches[2])"}
}

Wsl "cd '$wroot/app/storage_dynamic59' && make clean OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0 && make host-test all OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0"
Wsl "cd '$wroot' && cc -DOF_PC -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined -fno-omit-frame-pointer -I.integration/openfpgaos/src/firmware/api app/storage_dynamic59/main.c -o build/storage-dynamic59-app/pc-ui-san && build/storage-dynamic59-app/pc-ui-san && cc -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined -fno-omit-frame-pointer app/storage_dynamic59/test_dynamic59.c -o build/storage-dynamic59-app/logic-san && build/storage-dynamic59-app/logic-san"
Wsl "cd '$wroot' && tests/run-storage-audit-rtl.sh"
$rtlLog=Get-Content -Raw (Join-Path $root 'build/rtl/storage-audit/test.log')
if($rtlLog-notmatch 'Bridge audit: 91 PASS, 0 FAIL'){throw 'Parameter audit RTL regression failed'}
$src=Get-Content -Raw (Join-Path $root 'app/storage_dynamic59/main.c')
if($src-match '\bof_output_write\s*\(' -or $src-match '\bof_output_flush\s*\(' -or $src-match '\bof_cart_' -or $src-match '0x0188u'){throw 'Forbidden write/flush/cartridge operation'}
$elf=[IO.File]::ReadAllBytes($app)
if($elf[0]-ne 0x7f -or $elf[4]-ne 1 -or $elf[5]-ne 1 -or [BitConverter]::ToUInt16($elf,18)-ne 243 -or ![Text.Encoding]::ASCII.GetString($elf).Contains('TF OPEN TRACE smoke.59')){throw 'Wrong app ELF'}
$load=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && riscv64-unknown-elf-readelf -l build/storage-dynamic59-app/app.elf")-join "`n"
if($LASTEXITCODE-ne 0 -or $load-notmatch 'LOAD\s+0x[0-9a-f]+\s+0x10400000\s+0x10400000' -or $load-match 'INTERP|DYNAMIC'){throw 'Unexpected app load map'}
$ui=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && build/storage-dynamic59-app/pc-ui --demo")
if($LASTEXITCODE-ne 0){throw 'UI fixture failed'}
$ui=$ui|ForEach-Object{[regex]::Replace([string]$_,'\x1b\[[0-9;]*[A-Za-z]','')}
$pages=0;$rows=0;$maxRows=0;$maxWidth=0
foreach($line in $ui){if($line-eq ' TF OPEN TRACE smoke.59'){$maxRows=[Math]::Max($maxRows,$rows);$rows=0;++$pages};++$rows;$maxWidth=[Math]::Max($maxWidth,$line.Length);if($line.Length-gt 40){throw "UI line too wide: $line"}}
$maxRows=[Math]::Max($maxRows,$rows)
if($pages-ne 4 -or $maxRows-gt 18){throw "UI shape failed pages=$pages rows=$maxRows"}

foreach($stage in @('map','fit','asm','mif_update')){if((Get-Content -Raw (Join-Path $fpga "output_files/ap_core.$stage.rpt"))-notmatch 'was successful\. 0 errors'){throw "FPGA stage failed $stage"}}
$map=Get-Content -Raw (Join-Path $fpga 'output_files/ap_core.map.rpt')
foreach($ram in @('events_rtl_0','last_tx_rtl_0')){if($map-notmatch "pct_bridge_audit:storage_events\|altsyncram:$ram"){throw "Missing inferred audit RAM $ram"}}
$sta=Get-Content -Raw (Join-Path $fpga 'output_files/ap_core.sta.rpt');$slack=@{}
foreach($kind in @('setup','hold')){$m=[regex]::Matches($sta,"Worst-case $kind slack is\s+(-?\d+(?:\.\d+)?)");if(!$m.Count){throw "Missing $kind"};$slack[$kind]=($m|ForEach-Object{[double]$_.Groups[1].Value}|Measure-Object -Minimum).Minimum;if($slack[$kind]-le 0){throw "Timing failed $kind $($slack[$kind])"}}
foreach($line in Get-Content (Join-Path $fpga 'SOURCE_SHA256.txt')){if($line-notmatch '^([0-9a-f]{64})  (.+)$' -or (Sha $Matches[2])-ne $Matches[1]){throw "FPGA source drift $line"}}

$sd=Join-Path $bundle 'SD_ROOT';New-Item -ItemType Directory -Path $bundle,$sd|Out-Null
$omit=@('Assets/chisburner/common/chisburner58.elf','Assets/chisburner/common/chisburner58.ini','Assets/chisburner/ChisBurner.ChisBurner/TF Dynamic 58.json','Saves/chisburner/common/DYNAMIC58-DIRECTORY.txt')
$basePrefix=[IO.Path]::GetFullPath($base).TrimEnd('\')+'\'
foreach($f in Get-ChildItem -LiteralPath $base -Recurse -File){$rel=$f.FullName.Substring($basePrefix.Length).Replace('\','/');if($rel-in $omit){continue};$dest=Join-Path $sd $rel;New-Item -ItemType Directory -Force (Split-Path $dest -Parent)|Out-Null;Copy-Item -LiteralPath $f.FullName -Destination $dest}
$osRel='Assets/chisburner/common/os.bin';$rbfRel='Cores/ChisBurner.ChisBurner/chis.rbf_r'
$osHash='17c8cae47bd24869e5fbaa01514ffcd0f5ffc77fd514f5305bd3eeb1a719f7fa'
if((Sha (Join-Path $sd $osRel))-ne $osHash){throw 'OS must remain smoke58 byte-identical'}
$osCrc=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes((Join-Path $sd $osRel)))
$common=Join-Path $sd 'Assets/chisburner/common';Copy-Item -LiteralPath $app -Destination (Join-Path $common 'chisburner59.elf')
[IO.File]::WriteAllText((Join-Path $common 'chisburner59.ini'),"[os]`nELF=chisburner59.elf`nARGS=`n",$utf8)
$instanceDir=Join-Path $sd 'Assets/chisburner/ChisBurner.ChisBurner';New-Item -ItemType Directory -Force $instanceDir|Out-Null
$instance=[ordered]@{instance=[ordered]@{magic='APF_VER_1';data_slots=@([ordered]@{id=1;filename='os.bin'},[ordered]@{id=2;filename='chisburner59.ini'},[ordered]@{id=3;filename='chisburner59.elf'})}}
[IO.File]::WriteAllText((Join-Path $instanceDir 'TF Open Trace 59.json'),($instance|ConvertTo-Json -Depth 10),$utf8)
$markerDir=Join-Path $sd 'Saves/chisburner/common';New-Item -ItemType Directory -Force $markerDir|Out-Null
[IO.File]::WriteAllText((Join-Path $markerDir 'DYNAMIC59-DIRECTORY.txt'),"Parent directory marker only. dyn59-a7c9.bin is intentionally absent.`n",$utf8)
$corePath=Join-Path $sd 'Cores/ChisBurner.ChisBurner/core.json';$core=Get-Content -Raw $corePath|ConvertFrom-Json
$core.core.metadata.version='0.1.0-smoke.59-open-param-trace';$core.core.metadata.date_release='2026-09-04';$core.core.framework.version_required='2.3';$core.core.framework.hardware.cartridge_adapter=-1
[IO.File]::WriteAllText($corePath,($core|ConvertTo-Json -Depth 25),$utf8)
$dataPath=Join-Path $sd 'Cores/ChisBurner.ChisBurner/data.json';$data=Get-Content -Raw $dataPath|ConvertFrom-Json
foreach($slot in $data.data.data_slots){switch([int]$slot.id){2{$slot.filename='chisburner59.ini'}3{$slot.filename='chisburner59.elf'}4{$slot.name='Dynamic Output';$slot.required=$false;$slot.parameters=0;$slot.deferload=$true;$slot.extensions=@('bin');foreach($p in @('filename','nonvolatile','address','size_exact','size_maximum','secondary')){$slot.PSObject.Properties.Remove($p)}}}}
[IO.File]::WriteAllText($dataPath,($data|ConvertTo-Json -Depth 25),$utf8)
[IO.File]::WriteAllText((Join-Path $sd 'Cores/ChisBurner.ChisBurner/info.txt'),"smoke.59: 0192 OPENFILE parameter visibility trace. No cartridge, data write, or 0188 flush.`n",$utf8)

if(-not ('ChisBurner.Dynamic59.Reverse' -as [type])){Add-Type -TypeDefinition @'
namespace ChisBurner.Dynamic59 { public static class Reverse { public static byte[] Apply(byte[] a) { var t=new byte[256]; for(int i=0;i<256;i++){int x=i,y=0;for(int b=0;b<8;b++){y=(y<<1)|(x&1);x>>=1;}t[i]=(byte)y;} var o=new byte[a.Length];for(int i=0;i<a.Length;i++)o[i]=t[a[i]];return o; } } }
'@}
$rawRbf=[IO.File]::ReadAllBytes((Join-Path $fpga 'output_files/ap_core.rbf'));$rbfR=[ChisBurner.Dynamic59.Reverse]::Apply($rawRbf)
if([BitConverter]::ToString([ChisBurner.Dynamic59.Reverse]::Apply([byte[]](1,128,18,170)))-ne '80-01-48-55'){throw 'RBF reversal vector failed'}
$round=[ChisBurner.Dynamic59.Reverse]::Apply($rbfR);if($round.Length-ne $rawRbf.Length){throw 'RBF length changed'};for($i=0;$i-lt $rawRbf.Length;++$i){if($round[$i]-ne $rawRbf[$i]){throw "RBF reversal round-trip failed at $i"}}
[IO.File]::WriteAllBytes((Join-Path $sd $rbfRel),$rbfR)

$target=Join-Path $sd 'Saves/chisburner/common/dyn59-a7c9.bin';if(Test-Path -LiteralPath $target){throw 'Dynamic target must not be shipped'}
foreach($json in Get-ChildItem -LiteralPath $sd -Filter '*.json' -Recurse -File){$null=Get-Content -Raw $json.FullName|ConvertFrom-Json}
Copy-Item -LiteralPath (Join-Path $root 'app/storage_dynamic59/README_TEST.txt') -Destination $bundle
Copy-Item -LiteralPath (Join-Path $fpga 'SOURCE_SHA256.txt') -Destination (Join-Path $bundle 'FPGA_SOURCE_SHA256.txt')
[IO.File]::WriteAllLines((Join-Path $bundle 'UI_FIXTURES_SYNTHETIC.txt'),[string[]]$ui,$utf8)
$sources=@('app/storage_dynamic59/main.c','app/storage_dynamic59/dynamic59.h','app/storage_dynamic59/test_dynamic59.c','app/storage_dynamic59/Makefile','app/storage_dynamic59/README_TEST.txt','app/storage_snapshot_forensics/snapshot.h','core/storage/pct_bridge_audit.v','tests/rtl/tb_storage_audit.cpp','tests/build-dynamic59.ps1','tests/build-dynamic59-fpga.ps1','.integration/openfpgaos/src/firmware/api/of_output.h','.integration/openfpgaos/src/firmware/api/of_bridge_audit.h','.integration/openfpgaos/src/firmware/api/of_bridge_trace.h')
[IO.File]::WriteAllLines((Join-Path $bundle 'SOFTWARE_SOURCE_SHA256.txt'),[string[]]@($sources|ForEach-Object{"$(Sha (Join-Path $root $_))  $_"}),$utf8)
$proof=@"
smoke=59-openfile-parameter-visibility
hardware_result=PENDING
base=smoke58 OS and JSON layout; new traced FPGA and app
os_sha256=$osHash
os_internal_crc=$('{0:x8}' -f $osCrc.ComputedCrc)
fpga_seed=$Seed
fpga_setup_slack_ns=$($slack.setup)
fpga_hold_slack_ns=$($slack.hold)
failed_fpga_seed=3 setup_slack_ns=-0.189;not_packaged
rtl_parameter_trace=91_PASS_0_FAIL
target_path=/Saves/chisburner/common/dyn59-a7c9.bin
target_file_shipped=NO
target_size=8192
openfile_param_pointer=20400000
openfile_param_bytes=264
expected_wire_word0=2F536176
expected_wire_word1=65732F66
expected_wire_flags=03000000
expected_wire_size=00200000
data_write_calls=ZERO
0188_flush_calls=ZERO
cartridge_access=ZERO;adapter disabled
ui_pages=$pages;max_columns=$maxWidth;max_rows=$maxRows
asan_ubsan=PASS
app_rv32_build=PASS
rbf_raw_sha256=$(Sha (Join-Path $fpga 'output_files/ap_core.rbf'))
rbf_r_sha256=$(Sha (Join-Path $sd $rbfRel))
"@
[IO.File]::WriteAllText((Join-Path $bundle 'BUILD_PROVENANCE.txt'),$proof,$utf8)
$sdPrefix=[IO.Path]::GetFullPath($sd).TrimEnd('\')+'\';$hashes=@{}
$sums=Get-ChildItem -LiteralPath $sd -Recurse -File|Sort-Object FullName|ForEach-Object{$rel=$_.FullName.Substring($sdPrefix.Length).Replace('\','/');$h=Sha $_.FullName;$hashes["SD_ROOT/$rel"]=$h;"$h  $rel"}
[IO.File]::WriteAllLines((Join-Path $bundle 'SHA256SUMS.txt'),[string[]]$sums,$utf8)
Compress-Archive -Path (Join-Path $bundle '*') -DestinationPath $zip -CompressionLevel Optimal
$null=Assert-ChisBurnerZipOsIntegrity -ZipPath $zip
Add-Type -AssemblyName System.IO.Compression.FileSystem;$archive=[IO.Compression.ZipFile]::OpenRead($zip)
try{if($archive.Entries|Where-Object{$_.FullName-match 'dyn59-a7c9\.bin$'}){throw 'ZIP contains forbidden target'};foreach($name in $hashes.Keys){$e=$archive.GetEntry($name);if(!$e){throw "ZIP missing $name"};$s=$e.Open();$sha=[Security.Cryptography.SHA256]::Create();try{$actual=[BitConverter]::ToString($sha.ComputeHash($s)).Replace('-','').ToLowerInvariant();if($actual-ne $hashes[$name]){throw "ZIP mismatch $name"}}finally{$s.Dispose();$sha.Dispose()}}}finally{$archive.Dispose()}
"PASS: $zip"
"ZIP SHA256: $(Sha $zip)"
"Seed $Seed timing setup=$($slack.setup) hold=$($slack.hold); $($hashes.Count) payloads verified"
