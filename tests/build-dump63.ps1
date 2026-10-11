[CmdletBinding()]
param(
 [string]$Distro='Ubuntu-24.04',
 [string]$OpenFpgaSdk='/home/lin13/work/ONS_FPGA/openfpgaSDK'
)
$ErrorActionPreference='Stop'
. (Join-Path $PSScriptRoot 'os-image-integrity.ps1')
function Sha([string]$p){(Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash.ToLowerInvariant()}
function Wsl([string]$cmd){
 & wsl.exe -d $Distro -- bash -lc $cmd
 if($LASTEXITCODE-ne 0){throw "Build/test failed: $cmd"}
}

$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$wroot='/mnt/'+$root.Substring(0,1).ToLowerInvariant()+'/'+$root.Substring(3).Replace('\','/')
$baseBundle=Join-Path $root 'build/smoke/ChisBurner-smoke.57-emerald32-dump'
$base=Join-Path $baseBundle 'SD_ROOT'
$bundle=Join-Path $root 'build/smoke/ChisBurner-smoke.63-dynamic-emerald32-dump'
$zip="$bundle.zip"
$app=Join-Path $root 'build/dump63-app/app.elf'
$utf8=[Text.UTF8Encoding]::new($false)
$baseZipHash='dedebbf808cd8be6928024bb7b1cf8df2d26fcd6f2eaf04915912c6a7cc90cab'
$osHash='c428375acdec88847b1fc26df29a16948cd994430d028d39f13dfdda3b9c19a4'
$rbfHash='1a9ee605f61a52b586108c9ce8594ab6bf6210ab9f9889baccf8ba6c51d9d06a'
$targetRel='Saves/chisburner/common/dump57-32m.gba'
if((Test-Path -LiteralPath $bundle) -or (Test-Path -LiteralPath $zip)){
 throw 'Preserve existing smoke63 package'
}
if((Sha "$baseBundle.zip")-ne $baseZipHash){throw 'Wrong smoke57 base ZIP'}
foreach($line in Get-Content (Join-Path $baseBundle 'SHA256SUMS.txt')){
 if($line-notmatch '^([0-9a-f]{64})  (.+)$'){throw 'Bad smoke57 manifest'}
 if((Sha (Join-Path $base $Matches[2]))-ne $Matches[1]){
  throw "Changed smoke57 base $($Matches[2])"
 }
}
if((Sha (Join-Path $base 'Assets/chisburner/common/os.bin'))-ne $osHash){
 throw 'Wrong physically accepted smoke57 OS'
}
if((Sha (Join-Path $base 'Cores/ChisBurner.ChisBurner/chis.rbf_r'))-ne $rbfHash){
 throw 'Wrong physically accepted smoke57 RBF'
}

$rom='C:/Users/lin13/Downloads/[1MSRAM][FTC]口袋妖怪 - 宝石_中文典藏版[漫游&TGB联合汉化].gba'
$romSha='d1e41d5f8004de024391be96535bbc8952fb30a7b04b6e9c5a666d6927492dec'
if((Get-Item -LiteralPath $rom).Length-ne 33554432 -or (Sha $rom)-ne $romSha){
 throw 'Current Emerald reference changed'
}
if((Get-FileHash -LiteralPath $rom -Algorithm MD5).Hash-ne '872A1A05E39006F3BEA69EFAA641B3A4'){
 throw 'Current Emerald MD5 changed'
}

Wsl "cd '$wroot/app/gba_dump57' && make -f Makefile63 host-test all OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0"
Wsl "cd '$wroot' && cc -DOF_PC -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined -fno-omit-frame-pointer -I.integration/openfpgaos/src/firmware/api app/gba_dump57/main63.c app/gba_dump57/identify56_link.c app/gba_dump57/db55_link.c -o build/dump63-app/pc-ui-san && build/dump63-app/pc-ui-san >/dev/null"
Wsl "cd '$wroot' && tests/run-storage-audit-rtl.sh"
$rtlLog=Get-Content -Raw (Join-Path $root 'build/rtl/storage-audit/test.log')
if($rtlLog-notmatch 'Bridge audit: 91 PASS, 0 FAIL'){
 throw 'Data-slot bridge audit regression'
}

$src=Get-Content -Raw (Join-Path $root 'app/gba_dump57/main63.c')
$dyn=Get-Content -Raw (Join-Path $root 'app/gba_dump57/dynamic63.h')
foreach($needle in @('IDENTIFYING CART','rc = identify_cart57();','rc = d63_open(0u, &host);',
                     'rc = d63_open(1u, &host);','D63_DS_COMMAND = 3u;',
                     'd63_make_open_param(param, create, TOTAL57)')){
 if(!$src.Contains($needle)){throw "Missing identify/dynamic sequence: $needle"}
}
foreach($needle in @('create ? 3u : 0u','create ? size : 0u',
                     '/Saves/chisburner/common/dump57-32m.gba')){
 if(!$dyn.Contains($needle)){throw "Missing dynamic parameter rule: $needle"}
}
$runtime=@(
 'app/gba_dump57/main63.c','app/gba_dump57/dynamic63.h',
 'app/gba_dump57/main.c','app/gba_dump57/dump57.h',
 'app/gba_dump57/read_block.h','app/gba_dump57/reference.h',
 'app/gba_dump57/identify56_link.c','app/gba_dump57/db55_link.c',
 'app/gba_ident56/identify56.c','app/gba_ident56/identify56.h',
 'app/gba_db55/db55.c','app/gba_db55/db55.h','app/gba_db55/db55_data.c',
 'app/gba_dump53/md5.h','app/storage_stream52/gate.h'
)
foreach($p in $runtime){
 $s=Get-Content -Raw (Join-Path $root $p)
 if($s-match '\bof_output_flush\s*\(' -or
    $s-match '\bof_cart_(write|erase|program)\s*\(' -or
    $s-match 'OF_CART_SPACE_(SAVE|RTC)'){
  throw "Forbidden mutation path in $p"
 }
}

$elf=[IO.File]::ReadAllBytes($app)
if($elf[0]-ne 0x7f -or $elf[4]-ne 1 -or $elf[5]-ne 1 -or
   [BitConverter]::ToUInt16($elf,18)-ne 243 -or
   ![Text.Encoding]::ASCII.GetString($elf).Contains('GBA DYNAMIC DUMP smoke.63')){
 throw 'Wrong application ELF'
}
$load=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && riscv64-unknown-elf-readelf -l build/dump63-app/app.elf")-join "`n"
if($LASTEXITCODE-ne 0 -or
   $load-notmatch 'LOAD\s+0x[0-9a-f]+\s+0x10400000\s+0x10400000' -or
   $load-match 'INTERP|DYNAMIC'){
 throw 'Unexpected application load map'
}
$ui=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && build/dump63-app/pc-ui")
if($LASTEXITCODE-ne 0){throw 'UI fixture failed'}
$ui=$ui|ForEach-Object{[regex]::Replace([string]$_,'\x1b\[[0-9;]*[A-Za-z]','')}
$pages=0;$rows=0;$maxRows=0;$maxWidth=0
foreach($line in $ui){
 if($line-eq ' GBA DYNAMIC DUMP smoke.63'){
  $maxRows=[Math]::Max($maxRows,$rows);$rows=0;++$pages
 }
 ++$rows
 $maxWidth=[Math]::Max($maxWidth,$line.Length)
 if($line.Length-gt 40){throw "UI line too wide: $line"}
}
$maxRows=[Math]::Max($maxRows,$rows)
if($pages-ne 5 -or $maxRows-gt 18){
 throw "UI shape failed pages=$pages rows=$maxRows"
}

$sd=Join-Path $bundle 'SD_ROOT'
New-Item -ItemType Directory -Path $bundle,$sd|Out-Null
$omit=@(
 'Assets/chisburner/common/chisburner57.elf',
 'Assets/chisburner/common/chisburner57.ini',
 'Assets/chisburner/ChisBurner.ChisBurner/dump57-32m.gba'
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
if((Sha (Join-Path $sd $osRel))-ne $osHash){throw 'Packaged OS changed'}
if((Sha (Join-Path $sd $rbfRel))-ne $rbfHash){throw 'Packaged RBF changed'}
$osCrc=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes((Join-Path $sd $osRel)))

$common=Join-Path $sd 'Assets/chisburner/common'
Copy-Item -LiteralPath $app -Destination (Join-Path $common 'chisburner63.elf')
[IO.File]::WriteAllText((Join-Path $common 'chisburner63.ini'),
 "[os]`nELF=chisburner63.elf`nARGS=`n",$utf8)
$saveDir=Join-Path $sd 'Saves/chisburner/common'
New-Item -ItemType Directory -Force $saveDir|Out-Null
[IO.File]::WriteAllText((Join-Path $saveDir 'DYNAMIC63-README.txt'),
 "dump57-32m.gba is intentionally absent; smoke.63 creates it only after cartridge identification passes.`n",$utf8)
if(Test-Path -LiteralPath (Join-Path $sd $targetRel)){
 throw 'Dynamic output must not be shipped'
}

$corePath=Join-Path $sd 'Cores/ChisBurner.ChisBurner/core.json'
$core=Get-Content -Raw $corePath|ConvertFrom-Json
$core.core.metadata.version='0.1.0-smoke.63-dynamic-emerald32-dump'
$core.core.metadata.date_release='2026-09-04'
$core.core.framework.version_required='2.3'
if([int64]$core.core.framework.hardware.cartridge_adapter-ne 16777216){
 throw 'Cartridge adapter changed from smoke57'
}
[IO.File]::WriteAllText($corePath,($core|ConvertTo-Json -Depth 25),$utf8)
$dataPath=Join-Path $sd 'Cores/ChisBurner.ChisBurner/data.json'
$data=Get-Content -Raw $dataPath|ConvertFrom-Json
foreach($slot in $data.data.data_slots){switch([int]$slot.id){
 2{$slot.filename='chisburner63.ini'}
 3{$slot.filename='chisburner63.elf'}
 4{$slot.name='Dynamic GBA Dump 63';$slot.required=$false;$slot.parameters=0
   $slot.deferload=$true;$slot.extensions=@('gba')
   foreach($p in @('filename','nonvolatile','address','size_exact','size_maximum','secondary')){
    $slot.PSObject.Properties.Remove($p)
   }}
}}
[IO.File]::WriteAllText($dataPath,($data|ConvertTo-Json -Depth 25),$utf8)
[IO.File]::WriteAllText((Join-Path $sd 'Cores/ChisBurner.ChisBurner/info.txt'),
 "smoke.63: identify BPEE/S29GL256N first, then dynamic 0x0192 create and smoke.57 read-only 32 MiB dump; no 0x0188 flush.`n",$utf8)
foreach($json in Get-ChildItem -LiteralPath $sd -Filter '*.json' -Recurse -File){
 $null=Get-Content -Raw $json.FullName|ConvertFrom-Json
}
$slot4=$data.data.data_slots|Where-Object id -eq 4
if([int]$slot4.parameters-ne 0 -or !$slot4.deferload -or $slot4.filename -or
   $slot4.size_exact){throw 'Slot4 is not dynamic writable/optional'}

Copy-Item -LiteralPath (Join-Path $root 'app/gba_dump57/README_TEST63.txt') -Destination $bundle
Copy-Item -LiteralPath (Join-Path $root 'tests/Compare-ROM57.ps1') -Destination (Join-Path $bundle 'Compare-ROM63.ps1')
Copy-Item -LiteralPath (Join-Path $baseBundle 'FPGA_SOURCE_SHA256.txt') -Destination $bundle
[IO.File]::WriteAllLines((Join-Path $bundle 'UI_FIXTURES_SYNTHETIC.txt'),[string[]]$ui,$utf8)
$sources=@($runtime+@(
 'app/gba_dump57/test_dynamic63.c','app/gba_dump57/test_dump57.c',
 'app/gba_dump57/Makefile63','app/gba_dump57/README_TEST63.txt',
 'tests/build-dump63.ps1','tests/Compare-ROM57.ps1',
 '.integration/openfpgaos/src/firmware/os/targets/pocket/dump57_services.c',
 '.integration/openfpgaos/src/firmware/api/of_cart.h',
 '.integration/openfpgaos/src/firmware/api/of_file.h',
 '.integration/openfpgaos/src/firmware/api/of_output.h'
))|Select-Object -Unique
[IO.File]::WriteAllLines((Join-Path $bundle 'SOFTWARE_SOURCE_SHA256.txt'),
 [string[]]@($sources|ForEach-Object{"$(Sha (Join-Path $root $_))  $_"}),$utf8)
$proof=@"
smoke=63-identify-first-dynamic-emerald32-dump
hardware_result=PENDING
smoke57_base_zip_sha256=$baseZipHash
smoke57_hardware_result=32MiB dump size and both MD5 accepted by user
smoke62_hardware_result=0192 create plus 0184 write persisted across normal quit and cold boot without 0188
scope=current exact 33554432-byte BPEE translation on 32MiB ChisFlash only
source_sha256=$romSha
source_md5=872a1a05e39006f3bea69efaa641b3a4
source_full_crc32=d81fc708
source_first16MiB_crc32=a03dc885
ordering=OS capability probe;cartridge database and NOR identification;then dynamic TF open/create
pre_identification_TF_target_access=ZERO
target_path=/Saves/chisburner/common/dump57-32m.gba
target_file_shipped=NO
existing_probe=0192 flags0 size0;host0 opens;host3 triggers create
create=0192 flags3 size33554432;wire flags00000003 size02000000
new_file_boot_scan=SKIPPED
existing_file_boot_scan=FULL;verified ROM disables rewrite;unknown stops
TF_write=1024 x 32768 through physically accepted smoke57 output service
TF_readback=immediate 1024 blocks plus independent full verification
cartridge_read=32 bytes per command;normal budget 1048605
cartridge_write_save_rtc_erase_voltage_switch=structurally unavailable
0188_flush_calls=ZERO
os_sha256=$osHash;exact physically accepted smoke57 reuse
os_internal_crc=$('{0:x8}' -f $osCrc.ComputedCrc)
rbf_r_sha256=$rbfHash;exact physically accepted smoke57 reuse
dynamic_parameter_logic=ASan_UBSan_PASS
dump_full_source_and_fault_gates=ASan_UBSan_PASS
app_pc_ui=ASan_UBSan_PASS
bridge_audit_rtl=91_PASS_0_FAIL
app_rv32_build=PASS
ui_pages=$pages;max_columns=$maxWidth;max_rows=$maxRows
"@
[IO.File]::WriteAllText((Join-Path $bundle 'BUILD_PROVENANCE.txt'),$proof,$utf8)
if((Sha $rom)-ne $romSha){throw 'Reference modified during build'}

$sdPrefix=[IO.Path]::GetFullPath($sd).TrimEnd('\')+'\'
$hashes=@{}
$sums=Get-ChildItem -LiteralPath $sd -Recurse -File|Sort-Object FullName|ForEach-Object{
 $rel=$_.FullName.Substring($sdPrefix.Length).Replace('\','/')
 $h=Sha $_.FullName;$hashes["SD_ROOT/$rel"]=$h;"$h  $rel"
}
if($hashes.Count-ne 16){throw "Unexpected payload count $($hashes.Count)"}
[IO.File]::WriteAllLines((Join-Path $bundle 'SHA256SUMS.txt'),[string[]]$sums,$utf8)
Compress-Archive -Path (Join-Path $bundle '*') -DestinationPath $zip -CompressionLevel Optimal
$null=Assert-ChisBurnerZipOsIntegrity -ZipPath $zip
Add-Type -AssemblyName System.IO.Compression.FileSystem
$archive=[IO.Compression.ZipFile]::OpenRead($zip)
try{
 if($archive.Entries|Where-Object{
    $_.FullName.Replace('\','/')-match '(^|/)dump57-32m\.gba$'}){
  throw 'ZIP contains forbidden 32 MiB seed/dynamic target'
 }
 foreach($name in $hashes.Keys){
  $entry=$archive.GetEntry($name);if(!$entry){throw "ZIP missing $name"}
  $stream=$entry.Open();$hash=[Security.Cryptography.SHA256]::Create()
  try{
   $actual=[BitConverter]::ToString($hash.ComputeHash($stream)).Replace('-','').ToLowerInvariant()
   if($actual-ne $hashes[$name]){throw "ZIP mismatch $name"}
  }finally{$stream.Dispose();$hash.Dispose()}
 }
}finally{$archive.Dispose()}
"PASS: $zip"
"ZIP SHA256: $(Sha $zip)"
"OS/RBF exact smoke57 reuse; dynamic target absent; $($hashes.Count) payloads verified"
