[CmdletBinding()]
param([string]$Distro='Ubuntu-24.04',[string]$OpenFpgaSdk='/home/lin13/work/ONS_FPGA/openfpgaSDK')
$ErrorActionPreference='Stop'
function Sha([string]$p){(Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash.ToLowerInvariant()}
function Wsl([string]$cmd){& wsl.exe -d $Distro -- bash -lc $cmd;if($LASTEXITCODE -ne 0){throw "Build/test failed: $cmd"}}
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$wroot='/mnt/'+$root.Substring(0,1).ToLowerInvariant()+'/'+$root.Substring(3).Replace('\','/')
$baseBundle=Join-Path $root 'build/smoke/ChisBurner-smoke.54-gba-dump'
$base=Join-Path $baseBundle 'SD_ROOT'
$bundle=Join-Path $root 'build/smoke/ChisBurner-smoke.55-gba-db'
$zip="$bundle.zip"
$db=Join-Path $root 'data/chisburner/db_AGB.json'
$upstream=Join-Path $root 'references/upstream/lesserkuma-flash-database'
$utf8=[Text.UTF8Encoding]::new($false)
if((Test-Path -LiteralPath $bundle) -or (Test-Path -LiteralPath $zip)){throw 'Preserve existing smoke55 package'}
if((Sha "$baseBundle.zip") -ne 'a24b449f7cb313b265a89539a778bd73aaeb6715cf1e962438fb17ec56002c72'){throw 'Wrong smoke54 base ZIP'}
foreach($line in Get-Content (Join-Path $baseBundle 'SHA256SUMS.txt')){
    if($line -notmatch '^([0-9a-f]{64})  (.+)$'){throw 'Bad smoke54 manifest'}
    if((Sha (Join-Path $base $Matches[2])) -ne $Matches[1]){throw "Changed smoke54 base file $($Matches[2])"}
}
if((git -C $upstream rev-parse HEAD) -ne '0272d8ef039b2e131a7278c1a3b6dd4ade5a7ecd'){throw 'Wrong third-party upstream commit'}
$upstreamDbCandidates=@(Get-ChildItem -LiteralPath $upstream -Recurse -File -Filter 'db_AGB.json')
if($upstreamDbCandidates.Count -ne 1){throw 'Pinned third-party database path is ambiguous'}
$upstreamDb=$upstreamDbCandidates[0].FullName
if((Sha $db) -ne 'd52de244166c5b609c27bfe3aa5c195ab73c261ce2ec677cb18e593b1f596a85' -or
   (Sha $db) -ne (Sha $upstreamDb)){throw 'Pinned GBA database mismatch'}
$parsed=Get-Content -Raw -LiteralPath $db|ConvertFrom-Json -AsHashtable
if($parsed.Count -ne 2923){throw "Database count $($parsed.Count)"}
$fieldCounts=@{};foreach($e in $parsed.Values){foreach($k in $e.Keys){$fieldCounts[$k]=1+($fieldCounts[$k]??0)}}
foreach($k in @('gn','ne','gc','rc','rs','st','ss','lg','rg')){if($fieldCounts[$k] -ne 2923){throw "Incomplete database field $k"}}
if($fieldCounts['rv'] -ne 147 -or $fieldCounts['3d'] -ne 44){throw 'Optional database field counts changed'}
& (Join-Path $root 'tools/generate-agb-db55.ps1')
if($LASTEXITCODE -ne 0){throw 'Database generation failed'}
Wsl "cd '$wroot/app/gba_db55' && make sdk-clean >/dev/null 2>&1 || true; make host-test all OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0"
$app=Join-Path $root 'build/db55-app/app.elf'
$rom='C:/Users/lin13/Desktop/game/逆转裁判.gba'
if((Get-Item -LiteralPath $rom).Length -ne 8705280){throw 'Reference ROM size changed'}
if((Sha $rom) -ne '0d0945fff2edd4d4ae627ed003ebb0050937ee689d606ccff89c4ff2c2638571'){throw 'Reference ROM content changed'}
$ui=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && build/db55-app/pc-ui")
if($LASTEXITCODE -ne 0){throw 'UI fixture failed'}
$ui=$ui|ForEach-Object{[regex]::Replace([string]$_,'\x1b\[[0-9;]*[A-Za-z]','')}
$pages=($ui|Where-Object{$_ -eq ' GBA DB smoke.55  1/3'}).Count+
       ($ui|Where-Object{$_ -eq ' GBA DB smoke.55  2/3'}).Count+
       ($ui|Where-Object{$_ -eq ' GBA DB smoke.55  3/3'}).Count
$width=($ui|ForEach-Object{$_.Length}|Measure-Object -Maximum).Maximum
if($pages -ne 3 -or $width -gt 40){throw "UI shape pages=$pages width=$width"}
$elf=[IO.File]::ReadAllBytes($app)
if($elf[0] -ne 0x7f -or $elf[4] -ne 1 -or $elf[5] -ne 1 -or [BitConverter]::ToUInt16($elf,18) -ne 243 -or
   ![Text.Encoding]::ASCII.GetString($elf).Contains('GBA DB smoke.55')){throw 'Wrong application ELF'}
$load=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && riscv64-unknown-elf-readelf -l build/db55-app/app.elf") -join "`n"
if($load -notmatch 'LOAD\s+0x[0-9a-f]+\s+0x10400000\s+0x10400000' -or $load -match 'INTERP|DYNAMIC'){throw 'Unexpected ELF load map'}
$runtime=@('app/gba_db55/main.c','app/gba_db55/db55.c','app/gba_db55/db55.h','app/gba_db55/sha1_55.h','app/gba_db55/db55_data.c')
foreach($p in $runtime){$s=Get-Content -Raw -LiteralPath (Join-Path $root $p);if($s -match 'of_(output|file_write)|of_cart_(write|erase|program)|OF_CART_SPACE_(SAVE|RTC)'){throw "Mutation call in $p"}}

$sd=Join-Path $bundle 'SD_ROOT';New-Item -ItemType Directory -Path $bundle|Out-Null
$omit=@('Assets/chisburner/common/chisburner54.elf','Assets/chisburner/common/chisburner54.ini',
        'Assets/chisburner/ChisBurner.ChisBurner/dump54.gba')
$prefix=[IO.Path]::GetFullPath($base).TrimEnd('\')+'\'
foreach($f in Get-ChildItem -LiteralPath $base -Recurse -File){
    $rel=$f.FullName.Substring($prefix.Length).Replace('\','/');if($rel -in $omit){continue}
    $dest=Join-Path $sd $rel;New-Item -ItemType Directory -Force (Split-Path $dest -Parent)|Out-Null
    Copy-Item -LiteralPath $f.FullName -Destination $dest
}
$common=Join-Path $sd 'Assets/chisburner/common';Copy-Item -LiteralPath $app -Destination (Join-Path $common 'chisburner55.elf')
[IO.File]::WriteAllText((Join-Path $common 'chisburner55.ini'),"[os]`nELF=chisburner55.elf`nARGS=`n",$utf8)
Copy-Item -LiteralPath $db -Destination (Join-Path $common 'db_AGB.json')
$corePath=Join-Path $sd 'Cores/ChisBurner.ChisBurner/core.json'
$core=Get-Content -Raw -LiteralPath $corePath|ConvertFrom-Json
$core.core.metadata.version='0.1.0-smoke.55-gba-db';$core.core.metadata.date_release='2026-09-04'
[IO.File]::WriteAllText($corePath,($core|ConvertTo-Json -Depth 25),$utf8)
$dataPath=Join-Path $sd 'Cores/ChisBurner.ChisBurner/data.json'
$data=Get-Content -Raw -LiteralPath $dataPath|ConvertFrom-Json
foreach($slot in $data.data.data_slots){
    switch([int]$slot.id){
        2 {$slot.filename='chisburner55.ini'}
        3 {$slot.filename='chisburner55.elf'}
        4 {$slot.name='Unused';$slot.parameters=0;$slot.extensions=@('bin');$slot.PSObject.Properties.Remove('filename');$slot.PSObject.Properties.Remove('size_exact')}
    }
}
[IO.File]::WriteAllText($dataPath,($data|ConvertTo-Json -Depth 25),$utf8)
foreach($json in Get-ChildItem -LiteralPath $sd -Filter '*.json' -Recurse -File){$null=Get-Content -Raw -LiteralPath $json.FullName|ConvertFrom-Json}
$osRel='Assets/chisburner/common/os.bin';$rbfRel='Cores/ChisBurner.ChisBurner/chis.rbf_r'
if((Sha (Join-Path $sd $osRel)) -ne (Sha (Join-Path $base $osRel)) -or
   (Sha (Join-Path $sd $rbfRel)) -ne (Sha (Join-Path $base $rbfRel))){throw 'OS/RBF must be byte-identical to smoke54'}
if(Test-Path (Join-Path $sd 'Saves')){throw 'No Saves directory allowed'}
Copy-Item -LiteralPath (Join-Path $root 'app/gba_db55/README_TEST.txt') -Destination $bundle
Copy-Item -LiteralPath (Join-Path $baseBundle 'FPGA_SOURCE_SHA256.txt') -Destination $bundle
[IO.File]::WriteAllLines((Join-Path $bundle 'UI_FIXTURES_SYNTHETIC.txt'),[string[]]$ui,$utf8)
$sources=@($runtime+@('app/gba_db55/test_db55.c','app/gba_db55/Makefile','app/gba_db55/README_TEST.txt',
  'tools/generate-agb-db55.ps1','tests/build-db55.ps1','docs/gba-database.md','data/chisburner/db_AGB.json'))
[IO.File]::WriteAllLines((Join-Path $bundle 'SOFTWARE_SOURCE_SHA256.txt'),[string[]]@($sources|ForEach-Object{"$(Sha (Join-Path $root $_))  $_"}),$utf8)
$proof=@"
smoke=55-gba-database-identification
hardware_result=PENDING;database lookup not yet tested on Pocket
third_party_db_commit=0272d8ef039b2e131a7278c1a3b6dd4ade5a7ecd
db_AGB_sha256=$(Sha $db)
db_entries=2923
required_fields_gn_ne_gc_rc_rs_st_ss_lg_rg=2923_each
optional_rv=147
optional_3d=44
runtime_entry_bytes=40
runtime_index_bytes=$([int]2923*40)
matching=SHA1 first 0x180 bytes;valid M game code uses first 0x100 bytes
known_policy=database rs exact;post-dump database rc required
unknown_policy=33554432-byte full GBA window
crc_mismatch_policy=do not silently accept exact length;offer full 32 MiB preservation dump
current_ROM_header_sha1=ae78cb07cb1ca398cd34379a67cf7e01be79f0f4
current_ROM_database_match=NO
current_ROM_planned_fallback_bytes=33554432
SHA1_vectors_full_index_known_3D_current_unknown_tests=PASS ASan UBSan
SDK_app_build=PASS
app_load_map=0x10400000 SDRAM;no dynamic interpreter
OS_and_RBF=byte-identical accepted smoke54
runtime_cart_access=24 aligned 32-byte ROM reads at 0x000..0x17f;then safe idle
cartridge_ROM_write_save_RTC_erase_program_calls=ABSENT
TF_output_write_and_flush_calls=ABSENT
dump_performed=NO
canonical_full_JSON_in_package=YES
UI_pages=$pages
UI_max_columns=$width
"@
[IO.File]::WriteAllText((Join-Path $bundle 'BUILD_PROVENANCE.txt'),$proof,$utf8)
$sdPrefix=[IO.Path]::GetFullPath($sd).TrimEnd('\')+'\';$hashes=@{}
$sums=Get-ChildItem -LiteralPath $sd -Recurse -File|Sort-Object FullName|ForEach-Object{
    $rel=$_.FullName.Substring($sdPrefix.Length).Replace('\','/');$h=Sha $_.FullName;$hashes["SD_ROOT/$rel"]=$h;"$h  $rel"
}
[IO.File]::WriteAllLines((Join-Path $bundle 'SHA256SUMS.txt'),[string[]]$sums,$utf8)
Compress-Archive -Path (Join-Path $bundle '*') -DestinationPath $zip -CompressionLevel Optimal
Add-Type -AssemblyName System.IO.Compression.FileSystem
$archive=[IO.Compression.ZipFile]::OpenRead($zip)
try{foreach($name in $hashes.Keys){$e=$archive.GetEntry($name);if(!$e){throw "Missing ZIP payload $name"};$s=$e.Open();$h=[Security.Cryptography.SHA256]::Create();try{$actual=[BitConverter]::ToString($h.ComputeHash($s)).Replace('-','').ToLowerInvariant();if($actual -ne $hashes[$name]){throw "ZIP mismatch $name"}}finally{$s.Dispose();$h.Dispose()}}}finally{$archive.Dispose()}
"PASS: $zip"
"ZIP SHA256: $(Sha $zip)"
"Database 2923 entries; app $($elf.Length) bytes; OS/RBF unchanged; $($hashes.Count) payload hashes verified"
