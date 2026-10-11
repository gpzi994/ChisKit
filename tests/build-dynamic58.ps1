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
$baseBundle=Join-Path $root 'build/smoke/ChisBurner-storage-flush-smoke.49B-fullfile-control'
$base=Join-Path $baseBundle 'SD_ROOT'
$bundle=Join-Path $root 'build/smoke/ChisBurner-smoke.58-dynamic-create'
$zip="$bundle.zip"
$app=Join-Path $root 'build/storage-dynamic58-app/app.elf'
$utf8=[Text.UTF8Encoding]::new($false)
$baseZipHash='02fa3a274c6be5491fb17a1a24e214641af8d0e6a4c67c000831b66803be1a17'
$osHash='17c8cae47bd24869e5fbaa01514ffcd0f5ffc77fd514f5305bd3eeb1a719f7fa'
$rbfHash='b0c016f3c8956263aef31d8a934b366413a45885d0f7e72080bd605ae2197223'
if((Test-Path -LiteralPath $bundle) -or (Test-Path -LiteralPath $zip)){throw 'Preserve existing smoke58 package'}
if((Sha "$baseBundle.zip")-ne $baseZipHash){throw 'Wrong smoke49B base ZIP'}
foreach($line in Get-Content (Join-Path $baseBundle 'SHA256SUMS.txt')){
    if($line-notmatch '^([0-9a-f]{64})  (.+)$'){throw 'Bad base manifest'}
    if((Sha (Join-Path $base $Matches[2]))-ne $Matches[1]){throw "Changed smoke49B base $($Matches[2])"}
}
$osRel='Assets/chisburner/common/os.bin'
$rbfRel='Cores/ChisBurner.ChisBurner/chis.rbf_r'
if((Sha (Join-Path $base $osRel))-ne $osHash -or (Sha (Join-Path $base $rbfRel))-ne $rbfHash){throw 'Wrong traced OS/RBF base'}
$osCrc=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes((Join-Path $base $osRel)))

Wsl "cd '$wroot/app/storage_dynamic58' && make clean OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0 && make host-test all OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0"
Wsl "cd '$wroot' && cc -DOF_PC -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined -fno-omit-frame-pointer -I.integration/openfpgaos/src/firmware/api app/storage_dynamic58/main.c -o build/storage-dynamic58-app/pc-ui-san && build/storage-dynamic58-app/pc-ui-san && cc -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined -fno-omit-frame-pointer app/storage_dynamic58/test_dynamic58.c -o build/storage-dynamic58-app/logic-san && build/storage-dynamic58-app/logic-san"
$src=Get-Content -Raw (Join-Path $root 'app/storage_dynamic58/main.c')
if($src-match '\bof_output_flush\s*\(' -or $src-match '\bof_cart_' -or $src-match '0x0188u'){throw 'Forbidden FLUSH or cartridge operation in app'}
$elf=[IO.File]::ReadAllBytes($app)
if($elf[0]-ne 0x7f -or $elf[4]-ne 1 -or $elf[5]-ne 1 -or [BitConverter]::ToUInt16($elf,18)-ne 243 -or
   ![Text.Encoding]::ASCII.GetString($elf).Contains('TF DYNAMIC smoke.58')){throw 'Wrong app ELF'}
$load=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && riscv64-unknown-elf-readelf -l build/storage-dynamic58-app/app.elf")-join "`n"
if($LASTEXITCODE-ne 0 -or $load-notmatch 'LOAD\s+0x[0-9a-f]+\s+0x10400000\s+0x10400000' -or $load-match 'INTERP|DYNAMIC'){throw 'Unexpected app load map'}
$ui=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && build/storage-dynamic58-app/pc-ui --demo")
if($LASTEXITCODE-ne 0){throw 'UI fixture failed'}
$ui=$ui|ForEach-Object{[regex]::Replace([string]$_,'\x1b\[[0-9;]*[A-Za-z]','')}
$pages=0;$rows=0;$maxRows=0;$maxWidth=0
foreach($line in $ui){
    if($line-eq ' TF DYNAMIC smoke.58'){$maxRows=[Math]::Max($maxRows,$rows);$rows=0;++$pages}
    ++$rows;$maxWidth=[Math]::Max($maxWidth,$line.Length)
    if($line.Length-gt 40){throw "UI line too wide: $line"}
}
$maxRows=[Math]::Max($maxRows,$rows)
if($pages-ne 5 -or $maxRows-gt 18){throw "UI shape failed pages=$pages rows=$maxRows"}
foreach($expected in @('HOST OPEN: CREATED (HOST OK1)','HOST OPEN: EXISTING (HOST OK0)','DYNAMIC FILE PERSISTENCE: PASS','APF 0188 FLUSH: NOT CALLED')){
    if($ui-cnotcontains $expected){throw "Missing UI fixture: $expected"}
}

$sd=Join-Path $bundle 'SD_ROOT'
New-Item -ItemType Directory -Path $bundle,$sd|Out-Null
foreach($rel in @($osRel,$rbfRel,
    'Cores/ChisBurner.ChisBurner/audio.json',
    'Cores/ChisBurner.ChisBurner/core.json',
    'Cores/ChisBurner.ChisBurner/data.json',
    'Cores/ChisBurner.ChisBurner/input.json',
    'Cores/ChisBurner.ChisBurner/interact.json',
    'Cores/ChisBurner.ChisBurner/loader.bin',
    'Cores/ChisBurner.ChisBurner/variants.json',
    'Cores/ChisBurner.ChisBurner/video.json',
    'Platforms/chisburner.json')){
    $dest=Join-Path $sd $rel;New-Item -ItemType Directory -Force (Split-Path $dest -Parent)|Out-Null
    Copy-Item -LiteralPath (Join-Path $base $rel) -Destination $dest
}
$common=Join-Path $sd 'Assets/chisburner/common';New-Item -ItemType Directory -Force $common|Out-Null
Copy-Item -LiteralPath $app -Destination (Join-Path $common 'chisburner58.elf')
[IO.File]::WriteAllText((Join-Path $common 'chisburner58.ini'),"[os]`nELF=chisburner58.elf`nARGS=`n",$utf8)
$instanceDir=Join-Path $sd 'Assets/chisburner/ChisBurner.ChisBurner';New-Item -ItemType Directory -Force $instanceDir|Out-Null
$instance=[ordered]@{instance=[ordered]@{magic='APF_VER_1';data_slots=@(
    [ordered]@{id=1;filename='os.bin'},
    [ordered]@{id=2;filename='chisburner58.ini'},
    [ordered]@{id=3;filename='chisburner58.elf'})}}
[IO.File]::WriteAllText((Join-Path $instanceDir 'TF Dynamic 58.json'),($instance|ConvertTo-Json -Depth 10),$utf8)
$markerDir=Join-Path $sd 'Saves/chisburner/common';New-Item -ItemType Directory -Force $markerDir|Out-Null
[IO.File]::WriteAllText((Join-Path $markerDir 'DYNAMIC58-DIRECTORY.txt'),"This marker only creates the parent directory. dyn58-a7c9.bin is intentionally absent.`n",$utf8)

$corePath=Join-Path $sd 'Cores/ChisBurner.ChisBurner/core.json'
$core=Get-Content -Raw $corePath|ConvertFrom-Json
$core.core.metadata.version='0.1.0-smoke.58-dynamic-create';$core.core.metadata.date_release='2026-09-04'
$core.core.framework.version_required='2.3';$core.core.framework.hardware.cartridge_adapter=-1
[IO.File]::WriteAllText($corePath,($core|ConvertTo-Json -Depth 25),$utf8)
$dataPath=Join-Path $sd 'Cores/ChisBurner.ChisBurner/data.json'
$data=Get-Content -Raw $dataPath|ConvertFrom-Json
foreach($slot in $data.data.data_slots){switch([int]$slot.id){
    2{$slot.filename='chisburner58.ini'}
    3{$slot.filename='chisburner58.elf'}
    4{$slot.name='Dynamic Output';$slot.required=$false;$slot.parameters=0;$slot.deferload=$true;$slot.extensions=@('bin');
      foreach($p in @('filename','nonvolatile','address','size_exact','size_maximum','secondary')){$slot.PSObject.Properties.Remove($p)}}
}}
[IO.File]::WriteAllText($dataPath,($data|ConvertTo-Json -Depth 25),$utf8)
[IO.File]::WriteAllText((Join-Path $sd 'Cores/ChisBurner.ChisBurner/info.txt'),"smoke.58: isolated 0192 dynamic create plus 0184 write/readback. No cartridge and no 0188 flush.`n",$utf8)

$target=Join-Path $sd 'Saves/chisburner/common/dyn58-a7c9.bin'
if(Test-Path -LiteralPath $target){throw 'Dynamic target must not be shipped'}
if((Sha (Join-Path $sd $osRel))-ne $osHash -or (Sha (Join-Path $sd $rbfRel))-ne $rbfHash){throw 'Traced OS/RBF changed'}
$slot4=(Get-Content -Raw $dataPath|ConvertFrom-Json).data.data_slots|Where-Object id -eq 4
if(!$slot4.deferload -or [int]$slot4.parameters -ne 0 -or $slot4.PSObject.Properties.Name-contains 'filename' -or $slot4.PSObject.Properties.Name-contains 'size_exact'){throw 'Slot4 is not truly unbound/writable/deferred'}
foreach($json in Get-ChildItem -LiteralPath $sd -Filter '*.json' -Recurse -File){$null=Get-Content -Raw $json.FullName|ConvertFrom-Json}

$readme=@"
ChisBurner smoke.58 - true dynamic file create/write

This is a small isolated TF test. It never accesses the cartridge and never
issues APF target command 0x0188. The target file dyn58-a7c9.bin is deliberately
NOT included. The only shipped Saves file is a text marker that creates the
already-supported parent directory.

1. Merge every folder under SD_ROOT into the TF root. Do not copy individual
   files selectively. Cold power off/on.
2. In ChisBurner select:
   Assets/chisburner/ChisBurner.ChisBurner/TF Dynamic 58.json
   Confirm the screen says TF DYNAMIC smoke.58. No file command runs yet.
3. First run: press A once. This requests 0x0192 create+resize to exactly 8192
   bytes, freezes the host trace immediately after OPEN, writes eight 1024-byte
   blocks with 0x0184, then reads and compares every byte. Expected page 1:
   HOST OPEN: CREATED (HOST OK1), MATCH 8192/8192, CRC D46FA793, RESULT PASS.
   Page 2 must show HOST OK1 CREATED 1 and HOST OK0 OPENED 0.
4. Exit with the Pocket root-menu Quit command, then cold power off/on. Do NOT
   recopy SD_ROOT and do not edit/delete the new file.
5. Select TF Dynamic 58.json again and press B. B opens the existing file and
   performs read-only verification. Expected page 1: HOST OPEN: EXISTING
   (HOST OK0), CRC D46FA793, RESULT PASS, DYNAMIC FILE PERSISTENCE PASS.
   Page 2 must show HOST OK0 OPENED 1 and HOST OK1 CREATED 0.

If either run differs, send both pages. The trace recorder is single-use per
cold boot, so do not run A and B without the requested power cycle. The helper
Compare-Dynamic58.ps1 can verify the resulting TF file from Windows.

Why no 0x0188 here: prior smoke46-51 hardware traces already showed Pocket
polling 0x0188 hundreds of thousands of times without ever writing BUSY or OK.
This package isolates the officially documented 0x0192+0x0184 path. 0x0192
sets the final file length before the first data write, so an 0x0188 size update
is not required for this bounded test.
"@
[IO.File]::WriteAllText((Join-Path $bundle 'README_TEST.txt'),$readme,$utf8)
Copy-Item -LiteralPath (Join-Path $root 'tests/Compare-Dynamic58.ps1') -Destination $bundle
Copy-Item -LiteralPath (Join-Path $baseBundle 'FPGA_SOURCE_SHA256.txt') -Destination $bundle
[IO.File]::WriteAllLines((Join-Path $bundle 'UI_FIXTURES_SYNTHETIC.txt'),[string[]]$ui,$utf8)
$sources=@('app/storage_dynamic58/main.c','app/storage_dynamic58/dynamic58.h','app/storage_dynamic58/test_dynamic58.c','app/storage_dynamic58/Makefile','app/storage_snapshot_forensics/snapshot.h','.integration/openfpgaos/src/firmware/api/of_output.h','.integration/openfpgaos/src/firmware/api/of_bridge_audit.h','.integration/openfpgaos/src/firmware/api/of_bridge_trace.h','tests/Compare-Dynamic58.ps1','tests/build-dynamic58.ps1')
[IO.File]::WriteAllLines((Join-Path $bundle 'SOFTWARE_SOURCE_SHA256.txt'),[string[]]@($sources|ForEach-Object{"$(Sha (Join-Path $root $_))  $_"}),$utf8)
$proof=@"
smoke=58-dynamic-create-write-readback
hardware_result=PENDING
base=smoke49B traced OS/RBF
base_zip_sha256=$baseZipHash
os_sha256=$osHash
os_internal_crc=$('{0:x8}' -f $osCrc.ComputedCrc)
rbf_r_sha256=$rbfHash
framework_minimum=2.3 (0192 datatable size update fix)
slot4=declared writable,deferload,true unbound;no filename,size,nonvolatile,address
target_path=/Saves/chisburner/common/dyn58-a7c9.bin
target_file_shipped=NO
target_size=8192
target_sha256=b7708bc5b3bb230ceda2d59eb2250a6044203230c4bd14fc35068c59fc7c43c1
target_md5=bf81923a23254c3594e8d0ea20cf6ab7
target_crc32=d46fa793
first_run=A;0192 expected host result1;8x1024 0184 writes;full readback
second_run_after_quit_coldboot=B;0192 expected host result0;read-only persistence verify
0188_calls=ZERO
cartridge_access=ZERO;adapter disabled
trace=armed before OPEN and frozen immediately after OPEN
host_tests=PASS
asan_ubsan=PASS
app_rv32_build=PASS
ui_pages=$pages;max_columns=$maxWidth;max_rows=$maxRows
"@
[IO.File]::WriteAllText((Join-Path $bundle 'BUILD_PROVENANCE.txt'),$proof,$utf8)
$sdPrefix=[IO.Path]::GetFullPath($sd).TrimEnd('\')+'\';$hashes=@{}
$sums=Get-ChildItem -LiteralPath $sd -Recurse -File|Sort-Object FullName|ForEach-Object{
    $rel=$_.FullName.Substring($sdPrefix.Length).Replace('\','/');$h=Sha $_.FullName;$hashes["SD_ROOT/$rel"]=$h;"$h  $rel"
}
[IO.File]::WriteAllLines((Join-Path $bundle 'SHA256SUMS.txt'),[string[]]$sums,$utf8)
Compress-Archive -Path (Join-Path $bundle '*') -DestinationPath $zip -CompressionLevel Optimal
$null=Assert-ChisBurnerZipOsIntegrity -ZipPath $zip
Add-Type -AssemblyName System.IO.Compression.FileSystem
$archive=[IO.Compression.ZipFile]::OpenRead($zip)
try{
    if($archive.Entries|Where-Object{$_.FullName-match 'dyn58-a7c9\.bin$'}){throw 'ZIP illegally contains dynamic target'}
    foreach($name in $hashes.Keys){
        $entry=$archive.GetEntry($name);if(!$entry){throw "ZIP missing $name"}
        $stream=$entry.Open();$sha=[Security.Cryptography.SHA256]::Create()
        try{$actual=[BitConverter]::ToString($sha.ComputeHash($stream)).Replace('-','').ToLowerInvariant()}finally{$stream.Dispose();$sha.Dispose()}
        if($actual-ne $hashes[$name]){throw "ZIP mismatch $name"}
    }
}finally{$archive.Dispose()}
"PASS: $zip"
"ZIP SHA256: $(Sha $zip)"
"Dynamic target absent; traced OS/RBF pinned; all $($hashes.Count) SD payloads verified"
