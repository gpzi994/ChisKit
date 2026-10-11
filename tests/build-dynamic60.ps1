[CmdletBinding()]
param(
 [string]$Distro='Ubuntu-24.04',
 [string]$OpenFpgaSdk='/home/lin13/work/ONS_FPGA/openfpgaSDK'
)
$ErrorActionPreference='Stop'
. (Join-Path $PSScriptRoot 'os-image-integrity.ps1')
function Sha([string]$p){(Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash.ToLowerInvariant()}
function Wsl([string]$cmd){& wsl.exe -d $Distro -- bash -lc $cmd;if($LASTEXITCODE-ne 0){throw "Build/test failed: $cmd"}}
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$wroot='/mnt/'+$root.Substring(0,1).ToLowerInvariant()+'/'+$root.Substring(3).Replace('\','/')
$baseBundle=Join-Path $root 'build/smoke/ChisBurner-smoke.59-open-param-trace-final'
$base=Join-Path $baseBundle 'SD_ROOT'
$bundle=Join-Path $root 'build/smoke/ChisBurner-smoke.60-open-control'
$zip="$bundle.zip"
$app=Join-Path $root 'build/storage-dynamic60-app/app.elf'
$utf8=[Text.UTF8Encoding]::new($false)
$baseZipHash='884e69098f0f9616d75e143148478d36ac6d2d3f0db912e30e84b3c0a2d3d2a0'
$rbfHash='f423b06a2b406eda6479f4d2298badef29bc9da8556044ca2ef9f082a47e257d'
$osHash='17c8cae47bd24869e5fbaa01514ffcd0f5ffc77fd514f5305bd3eeb1a719f7fa'
if((Test-Path -LiteralPath $bundle) -or (Test-Path -LiteralPath $zip)){throw 'Preserve existing smoke60 package'}
if((Sha "$baseBundle.zip")-ne $baseZipHash){throw 'Wrong smoke59 base ZIP'}
foreach($line in Get-Content (Join-Path $baseBundle 'SHA256SUMS.txt')){
 if($line-notmatch '^([0-9a-f]{64})  (.+)$'){throw 'Bad base manifest'}
 if((Sha (Join-Path $base $Matches[2]))-ne $Matches[1]){throw "Changed smoke59 base $($Matches[2])"}
}

Wsl "cd '$wroot/app/storage_dynamic60' && make clean OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0 && make host-test all OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0"
Wsl "cd '$wroot' && cc -DOF_PC -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined -fno-omit-frame-pointer -I.integration/openfpgaos/src/firmware/api app/storage_dynamic60/main.c -o build/storage-dynamic60-app/pc-ui-san && build/storage-dynamic60-app/pc-ui-san && cc -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined -fno-omit-frame-pointer app/storage_dynamic60/test_dynamic60.c -o build/storage-dynamic60-app/logic-san && build/storage-dynamic60-app/logic-san"
Wsl "cd '$wroot' && tests/run-storage-audit-rtl.sh"
$rtlLog=Get-Content -Raw (Join-Path $root 'build/rtl/storage-audit/test.log')
if($rtlLog-notmatch 'Bridge audit: 91 PASS, 0 FAIL'){throw 'Parameter audit RTL regression failed'}
$src=Get-Content -Raw (Join-Path $root 'app/storage_dynamic60/main.c')
if($src-match '\bof_output_write\s*\(' -or $src-match '\bof_output_flush\s*\(' -or
   $src-match '\bof_cart_' -or $src-match '0x0188u'){throw 'Forbidden write/flush/cartridge operation'}
$elf=[IO.File]::ReadAllBytes($app)
if($elf[0]-ne 0x7f -or $elf[4]-ne 1 -or $elf[5]-ne 1 -or
   [BitConverter]::ToUInt16($elf,18)-ne 243 -or
   ![Text.Encoding]::ASCII.GetString($elf).Contains('TF OPEN CONTROL smoke.60')){throw 'Wrong app ELF'}
$load=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && riscv64-unknown-elf-readelf -l build/storage-dynamic60-app/app.elf")-join "`n"
if($LASTEXITCODE-ne 0 -or $load-notmatch 'LOAD\s+0x[0-9a-f]+\s+0x10400000\s+0x10400000' -or
   $load-match 'INTERP|DYNAMIC'){throw 'Unexpected app load map'}
$ui=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && build/storage-dynamic60-app/pc-ui --demo")
if($LASTEXITCODE-ne 0){throw 'UI fixture failed'}
$ui=$ui|ForEach-Object{[regex]::Replace([string]$_,'\x1b\[[0-9;]*[A-Za-z]','')}
$pages=0;$rows=0;$maxRows=0;$maxWidth=0
foreach($line in $ui){
 if($line-eq ' TF OPEN CONTROL smoke.60'){$maxRows=[Math]::Max($maxRows,$rows);$rows=0;++$pages}
 ++$rows;$maxWidth=[Math]::Max($maxWidth,$line.Length)
 if($line.Length-gt 40){throw "UI line too wide: $line"}
}
$maxRows=[Math]::Max($maxRows,$rows)
if($pages-ne 5 -or $maxRows-gt 18){throw "UI shape failed pages=$pages rows=$maxRows"}

$sd=Join-Path $bundle 'SD_ROOT'
New-Item -ItemType Directory -Path $bundle,$sd|Out-Null
$omit=@(
 'Assets/chisburner/common/chisburner59.elf',
 'Assets/chisburner/common/chisburner59.ini',
 'Assets/chisburner/ChisBurner.ChisBurner/TF Open Trace 59.json',
 'Saves/chisburner/common/DYNAMIC59-DIRECTORY.txt'
)
$basePrefix=[IO.Path]::GetFullPath($base).TrimEnd('\')+'\'
foreach($f in Get-ChildItem -LiteralPath $base -Recurse -File){
 $rel=$f.FullName.Substring($basePrefix.Length).Replace('\','/')
 if($rel-in $omit){continue}
 $dest=Join-Path $sd $rel
 New-Item -ItemType Directory -Force (Split-Path $dest -Parent)|Out-Null
 Copy-Item -LiteralPath $f.FullName -Destination $dest
}
$osRel='Assets/chisburner/common/os.bin'
$rbfRel='Cores/ChisBurner.ChisBurner/chis.rbf_r'
if((Sha (Join-Path $sd $osRel))-ne $osHash){throw 'OS changed from smoke59'}
if((Sha (Join-Path $sd $rbfRel))-ne $rbfHash){throw 'RBF changed from smoke59'}
$osCrc=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes((Join-Path $sd $osRel))
)
$common=Join-Path $sd 'Assets/chisburner/common'
Copy-Item -LiteralPath $app -Destination (Join-Path $common 'chisburner60.elf')
[IO.File]::WriteAllText((Join-Path $common 'chisburner60.ini'),"[os]`nELF=chisburner60.elf`nARGS=`n",$utf8)
$instanceDir=Join-Path $sd 'Assets/chisburner/ChisBurner.ChisBurner'
New-Item -ItemType Directory -Force $instanceDir|Out-Null
$instance=[ordered]@{instance=[ordered]@{magic='APF_VER_1';data_slots=@(
 [ordered]@{id=1;filename='os.bin'},
 [ordered]@{id=2;filename='chisburner60.ini'},
 [ordered]@{id=3;filename='chisburner60.elf'}
)}}
[IO.File]::WriteAllText((Join-Path $instanceDir 'TF Open Control 60.json'),
 ($instance|ConvertTo-Json -Depth 10),$utf8)

$saveDir=Join-Path $sd 'Saves/chisburner/common'
New-Item -ItemType Directory -Force $saveDir|Out-Null
$existing=New-Object byte[] 8192
for($i=0;$i-lt $existing.Length;++$i){$existing[$i]=[byte](($i*29+0x60)-band 0xff)}
[IO.File]::WriteAllBytes((Join-Path $saveDir 'dyn60-existing.bin'),$existing)
[IO.File]::WriteAllText((Join-Path $saveDir 'DYNAMIC60-CONTROL.txt'),
 "dyn60-existing.bin is the disposable 8192-byte control; dyn60-missing.bin is intentionally absent.`n",$utf8)

$corePath=Join-Path $sd 'Cores/ChisBurner.ChisBurner/core.json'
$core=Get-Content -Raw $corePath|ConvertFrom-Json
$core.core.metadata.version='0.1.0-smoke.60-open-control'
$core.core.metadata.date_release='2026-09-04'
$core.core.framework.version_required='2.3'
$core.core.framework.hardware.cartridge_adapter=-1
[IO.File]::WriteAllText($corePath,($core|ConvertTo-Json -Depth 25),$utf8)
$dataPath=Join-Path $sd 'Cores/ChisBurner.ChisBurner/data.json'
$data=Get-Content -Raw $dataPath|ConvertFrom-Json
foreach($slot in $data.data.data_slots){switch([int]$slot.id){
 2{$slot.filename='chisburner60.ini'}
 3{$slot.filename='chisburner60.elf'}
 4{$slot.name='Dynamic Output Control';$slot.required=$false;$slot.parameters=0;
   $slot.deferload=$true;$slot.extensions=@('bin')
   foreach($p in @('filename','nonvolatile','address','size_exact','size_maximum','secondary')){
    $slot.PSObject.Properties.Remove($p)
   }}
}}
[IO.File]::WriteAllText($dataPath,($data|ConvertTo-Json -Depth 25),$utf8)
[IO.File]::WriteAllText((Join-Path $sd 'Cores/ChisBurner.ChisBurner/info.txt'),
 "smoke.60: 0192 existing/missing control and repeated-parameter detail. No cartridge, 0184 data write, or 0188 flush.`n",$utf8)

$missing=Join-Path $saveDir 'dyn60-missing.bin'
if(Test-Path -LiteralPath $missing){throw 'Missing-mode target must not be shipped'}
if((Get-Item -LiteralPath (Join-Path $saveDir 'dyn60-existing.bin')).Length-ne 8192){throw 'Bad existing control size'}
foreach($json in Get-ChildItem -LiteralPath $sd -Filter '*.json' -Recurse -File){
 $null=Get-Content -Raw $json.FullName|ConvertFrom-Json
}
Copy-Item -LiteralPath (Join-Path $root 'app/storage_dynamic60/README_TEST.txt') -Destination $bundle
Copy-Item -LiteralPath (Join-Path $baseBundle 'FPGA_SOURCE_SHA256.txt') -Destination $bundle
[IO.File]::WriteAllLines((Join-Path $bundle 'UI_FIXTURES_SYNTHETIC.txt'),[string[]]$ui,$utf8)
$sources=@(
 'app/storage_dynamic60/main.c','app/storage_dynamic60/dynamic60.h',
 'app/storage_dynamic60/test_dynamic60.c','app/storage_dynamic60/Makefile',
 'app/storage_dynamic60/README_TEST.txt','app/storage_snapshot_forensics/snapshot.h',
 'core/storage/pct_bridge_audit.v','tests/rtl/tb_storage_audit.cpp',
 'tests/build-dynamic60.ps1','.integration/openfpgaos/src/firmware/api/of_output.h',
 '.integration/openfpgaos/src/firmware/api/of_bridge_audit.h',
 '.integration/openfpgaos/src/firmware/api/of_bridge_trace.h'
)
[IO.File]::WriteAllLines((Join-Path $bundle 'SOFTWARE_SOURCE_SHA256.txt'),
 [string[]]@($sources|ForEach-Object{"$(Sha (Join-Path $root $_))  $_"}),$utf8)
$proof=@"
smoke=60-openfile-existing-missing-control
hardware_result=PENDING
base_smoke59_zip_sha256=$baseZipHash
os_sha256=$osHash
os_internal_crc=$('{0:x8}' -f $osCrc.ComputedCrc)
rbf_r_sha256=$rbfHash
fpga_reused=smoke59 seed7 setup_slack_ns=0.783 hold_slack_ns=0.121
rtl_parameter_trace=91_PASS_0_FAIL
mode_b_path=/Saves/chisburner/common/dyn60-existing.bin
mode_b_file_shipped=YES;8192 bytes disposable fixture
mode_a_path=/Saves/chisburner/common/dyn60-missing.bin
mode_a_file_shipped=NO
openfile_param_pointer=20400000
openfile_param_bytes=264
operation_flags=create+resize
target_size=8192
0184_data_write_calls=ZERO
0188_flush_calls=ZERO
cartridge_access=ZERO;adapter disabled
ui_pages=$pages;max_columns=$maxWidth;max_rows=$maxRows
asan_ubsan=PASS
app_rv32_build=PASS
"@
[IO.File]::WriteAllText((Join-Path $bundle 'BUILD_PROVENANCE.txt'),$proof,$utf8)
$sdPrefix=[IO.Path]::GetFullPath($sd).TrimEnd('\')+'\';$hashes=@{}
$sums=Get-ChildItem -LiteralPath $sd -Recurse -File|Sort-Object FullName|ForEach-Object{
 $rel=$_.FullName.Substring($sdPrefix.Length).Replace('\','/')
 $h=Sha $_.FullName;$hashes["SD_ROOT/$rel"]=$h;"$h  $rel"
}
[IO.File]::WriteAllLines((Join-Path $bundle 'SHA256SUMS.txt'),[string[]]$sums,$utf8)
Compress-Archive -Path (Join-Path $bundle '*') -DestinationPath $zip -CompressionLevel Optimal
$null=Assert-ChisBurnerZipOsIntegrity -ZipPath $zip
Add-Type -AssemblyName System.IO.Compression.FileSystem
$archive=[IO.Compression.ZipFile]::OpenRead($zip)
try{
 if($archive.Entries|Where-Object{$_.FullName-match 'dyn60-missing\.bin$'}){throw 'ZIP contains missing-mode target'}
 $e=$archive.GetEntry('SD_ROOT/Saves/chisburner/common/dyn60-existing.bin')
 if(!$e -or $e.Length-ne 8192){throw 'ZIP existing control missing or wrong size'}
 foreach($name in $hashes.Keys){
  $e=$archive.GetEntry($name);if(!$e){throw "ZIP missing $name"}
  $s=$e.Open();$sha=[Security.Cryptography.SHA256]::Create()
  try{$actual=[BitConverter]::ToString($sha.ComputeHash($s)).Replace('-','').ToLowerInvariant()
   if($actual-ne $hashes[$name]){throw "ZIP mismatch $name"}
  }finally{$s.Dispose();$sha.Dispose()}
 }
}finally{$archive.Dispose()}
"PASS: $zip"
"ZIP SHA256: $(Sha $zip)"
"Reused smoke59 FPGA timing setup=0.783 hold=0.121; $($hashes.Count) payloads verified"
