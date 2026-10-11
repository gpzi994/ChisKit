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
$baseBundle=Join-Path $root 'build/smoke/ChisBurner-smoke.60-open-control'
$base=Join-Path $baseBundle 'SD_ROOT'
$bundle=Join-Path $root 'build/smoke/ChisBurner-smoke.62-direct-open-known-os'
$zip="$bundle.zip"
$app=Join-Path $root 'build/storage-dynamic62-app/app.elf'
$utf8=[Text.UTF8Encoding]::new($false)
$baseZipHash='3113cd6ac7248d3eec391db8c64d9ab423568a5aa1437f3b8d421ec46a8f410c'
$osHash='17c8cae47bd24869e5fbaa01514ffcd0f5ffc77fd514f5305bd3eeb1a719f7fa'
$rbfHash='f423b06a2b406eda6479f4d2298badef29bc9da8556044ca2ef9f082a47e257d'
if((Test-Path -LiteralPath $bundle) -or (Test-Path -LiteralPath $zip)){throw 'Preserve existing smoke62 package'}
if((Sha "$baseBundle.zip")-ne $baseZipHash){throw 'Wrong smoke60 base ZIP'}
foreach($line in Get-Content (Join-Path $baseBundle 'SHA256SUMS.txt')){
 if($line-notmatch '^([0-9a-f]{64})  (.+)$'){throw 'Bad base manifest'}
 if((Sha (Join-Path $base $Matches[2]))-ne $Matches[1]){throw "Changed smoke60 base $($Matches[2])"}
}

Wsl "cd '$wroot/app/storage_dynamic62' && make clean OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0 && make host-test all OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0"
Wsl "cd '$wroot' && cc -DOF_PC -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined -fno-omit-frame-pointer -I.integration/openfpgaos/src/firmware/api app/storage_dynamic62/main.c -o build/storage-dynamic62-app/pc-ui-san && build/storage-dynamic62-app/pc-ui-san && cc -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined -fno-omit-frame-pointer app/storage_dynamic62/test_dynamic62.c -o build/storage-dynamic62-app/logic-san && build/storage-dynamic62-app/logic-san"
Wsl "cd '$wroot' && tests/run-storage-audit-rtl.sh"
$rtlLog=Get-Content -Raw (Join-Path $root 'build/rtl/storage-audit/test.log')
if($rtlLog-notmatch 'Bridge audit: 91 PASS, 0 FAIL'){throw 'Parameter audit RTL regression failed'}
$src=Get-Content -Raw (Join-Path $root 'app/storage_dynamic62/main.c')
if($src-match '\bof_output_flush\s*\(' -or $src-match '\bof_cart_' -or
   $src-match '0x0188u'){throw 'Forbidden flush/cartridge operation'}
foreach($needle in @('D62_DS_COMMAND = command','d62_put_wire_word(param + 256u',
                     'd62_put_wire_word(param + 260u','d62_write_block',
                     'd62_read_block')){
 if(!$src.Contains($needle)){throw "Direct bridge path missing: $needle"}
}
$elf=[IO.File]::ReadAllBytes($app)
if($elf[0]-ne 0x7f -or $elf[4]-ne 1 -or $elf[5]-ne 1 -or
   [BitConverter]::ToUInt16($elf,18)-ne 243 -or
   ![Text.Encoding]::ASCII.GetString($elf).Contains('TF DIRECT CREATE smoke.62')){
 throw 'Wrong app ELF'
}
$load=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && riscv64-unknown-elf-readelf -l build/storage-dynamic62-app/app.elf")-join "`n"
if($LASTEXITCODE-ne 0 -or $load-notmatch 'LOAD\s+0x[0-9a-f]+\s+0x10400000\s+0x10400000' -or
   $load-match 'INTERP|DYNAMIC'){throw 'Unexpected app load map'}
$ui=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && build/storage-dynamic62-app/pc-ui --demo")
if($LASTEXITCODE-ne 0){throw 'UI fixture failed'}
$ui=$ui|ForEach-Object{[regex]::Replace([string]$_,'\x1b\[[0-9;]*[A-Za-z]','')}
$pages=0;$rows=0;$maxRows=0;$maxWidth=0
foreach($line in $ui){
 if($line-eq ' TF DIRECT CREATE smoke.62'){$maxRows=[Math]::Max($maxRows,$rows);$rows=0;++$pages}
 ++$rows;$maxWidth=[Math]::Max($maxWidth,$line.Length)
 if($line.Length-gt 40){throw "UI line too wide: $line"}
}
$maxRows=[Math]::Max($maxRows,$rows)
if($pages-ne 5 -or $maxRows-gt 18){throw "UI shape failed pages=$pages rows=$maxRows"}

$sd=Join-Path $bundle 'SD_ROOT'
New-Item -ItemType Directory -Path $bundle,$sd|Out-Null
$omit=@(
 'Assets/chisburner/common/chisburner60.elf',
 'Assets/chisburner/common/chisburner60.ini',
 'Assets/chisburner/ChisBurner.ChisBurner/TF Open Control 60.json',
 'Saves/chisburner/common/dyn60-existing.bin',
 'Saves/chisburner/common/DYNAMIC60-CONTROL.txt'
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
if((Sha (Join-Path $sd $osRel))-ne $osHash){throw 'Known-good OS changed from smoke60'}
if((Sha (Join-Path $sd $rbfRel))-ne $rbfHash){throw 'RBF changed from smoke60'}
$osCrc=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes((Join-Path $sd $osRel)))
$common=Join-Path $sd 'Assets/chisburner/common'
Copy-Item -LiteralPath $app -Destination (Join-Path $common 'chisburner62.elf')
[IO.File]::WriteAllText((Join-Path $common 'chisburner62.ini'),"[os]`nELF=chisburner62.elf`nARGS=`n",$utf8)
$instanceDir=Join-Path $sd 'Assets/chisburner/ChisBurner.ChisBurner'
New-Item -ItemType Directory -Force $instanceDir|Out-Null
$instance=[ordered]@{instance=[ordered]@{magic='APF_VER_1';data_slots=@(
 [ordered]@{id=1;filename='os.bin'},
 [ordered]@{id=2;filename='chisburner62.ini'},
 [ordered]@{id=3;filename='chisburner62.elf'}
)}}
[IO.File]::WriteAllText((Join-Path $instanceDir 'TF Direct Create 62.json'),
 ($instance|ConvertTo-Json -Depth 10),$utf8)

$saveDir=Join-Path $sd 'Saves/chisburner/common'
New-Item -ItemType Directory -Force $saveDir|Out-Null
[IO.File]::WriteAllText((Join-Path $saveDir 'DYNAMIC62-README.txt'),
 "dyn62-direct.bin is intentionally absent; smoke.62 must create it dynamically.`n",$utf8)
$target=Join-Path $saveDir 'dyn62-direct.bin'
if(Test-Path -LiteralPath $target){throw 'Dynamic target must not be shipped'}

$corePath=Join-Path $sd 'Cores/ChisBurner.ChisBurner/core.json'
$core=Get-Content -Raw $corePath|ConvertFrom-Json
$core.core.metadata.version='0.1.0-smoke.62-direct-open-known-os'
$core.core.metadata.date_release='2026-09-04'
$core.core.framework.version_required='2.3'
$core.core.framework.hardware.cartridge_adapter=-1
[IO.File]::WriteAllText($corePath,($core|ConvertTo-Json -Depth 25),$utf8)
$dataPath=Join-Path $sd 'Cores/ChisBurner.ChisBurner/data.json'
$data=Get-Content -Raw $dataPath|ConvertFrom-Json
foreach($slot in $data.data.data_slots){switch([int]$slot.id){
 2{$slot.filename='chisburner62.ini'}
 3{$slot.filename='chisburner62.elf'}
 4{$slot.name='Dynamic Direct Output';$slot.required=$false;$slot.parameters=0;
   $slot.deferload=$true;$slot.extensions=@('bin')
   foreach($p in @('filename','nonvolatile','address','size_exact','size_maximum','secondary')){
    $slot.PSObject.Properties.Remove($p)
   }}
}}
[IO.File]::WriteAllText($dataPath,($data|ConvertTo-Json -Depth 25),$utf8)
[IO.File]::WriteAllText((Join-Path $sd 'Cores/ChisBurner.ChisBurner/info.txt'),
 "smoke.62: known-good smoke.60 OS; app-direct 0192/0184/0180. No cartridge or 0188 flush.`n",$utf8)
foreach($json in Get-ChildItem -LiteralPath $sd -Filter '*.json' -Recurse -File){
 $null=Get-Content -Raw $json.FullName|ConvertFrom-Json
}
Copy-Item -LiteralPath (Join-Path $root 'app/storage_dynamic62/README_TEST.txt') -Destination $bundle
Copy-Item -LiteralPath (Join-Path $baseBundle 'FPGA_SOURCE_SHA256.txt') -Destination $bundle
[IO.File]::WriteAllLines((Join-Path $bundle 'UI_FIXTURES_SYNTHETIC.txt'),[string[]]$ui,$utf8)
$sources=@(
 'app/storage_dynamic62/main.c','app/storage_dynamic62/dynamic62.h',
 'app/storage_dynamic62/test_dynamic62.c','app/storage_dynamic62/Makefile',
 'app/storage_dynamic62/README_TEST.txt','app/storage_dynamic61/dynamic61.h',
 'app/storage_dynamic60/dynamic60.h','app/storage_snapshot_forensics/snapshot.h',
 'core/storage/pct_bridge_audit.v','tests/rtl/tb_storage_audit.cpp',
 'tests/build-dynamic62.ps1','.integration/openfpgaos/src/firmware/api/of_output.h',
 '.integration/openfpgaos/src/firmware/api/of_bridge_audit.h',
 '.integration/openfpgaos/src/firmware/api/of_bridge_trace.h'
)
[IO.File]::WriteAllLines((Join-Path $bundle 'SOFTWARE_SOURCE_SHA256.txt'),
 [string[]]@($sources|ForEach-Object{"$(Sha (Join-Path $root $_))  $_"}),$utf8)
$proof=@"
smoke=62-direct-open-known-good-os
hardware_result=PENDING
smoke61_status=RETIRED_BOOT_HALT_AFTER_LATE_INIT
base_smoke60_zip_sha256=$baseZipHash
os_sha256=$osHash;exact known-good smoke60 reuse
os_internal_crc=$('{0:x8}' -f $osCrc.ComputedCrc)
rbf_r_sha256=$rbfHash
fpga_reused=smoke59/60 seed7 setup_slack_ns=0.783 hold_slack_ns=0.121
rtl_parameter_trace=91_PASS_0_FAIL
target_path=/Saves/chisburner/common/dyn62-direct.bin
target_file_shipped=NO
operation=app-direct 0192 open; 0184 write; 0180 read
create_wire_flags=00000003
create_wire_size=00002000
existing_wire_flags=00000000
existing_wire_size=00000000
0184_data_write_calls=8 x 1024 bytes
0180_data_read_calls=8 x 1024 bytes
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
 if($archive.Entries|Where-Object{$_.FullName-match 'dyn62-direct\.bin$'}){throw 'ZIP contains dynamic target'}
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
"Known-good OS SHA256: $osHash"
"Reused FPGA timing setup=0.783 hold=0.121; $($hashes.Count) payloads verified"
