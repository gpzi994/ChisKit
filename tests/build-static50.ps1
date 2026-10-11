[CmdletBinding()]
param([string]$Distro='Ubuntu-24.04',
      [string]$OpenFpgaSdk='/home/lin13/work/ONS_FPGA/openfpgaSDK')
$ErrorActionPreference='Stop'
. (Join-Path $PSScriptRoot 'os-image-integrity.ps1')
function Sha([string]$p){(Get-FileHash -LiteralPath $p).Hash.ToLowerInvariant()}
function Wsl([string]$cmd){ & wsl.exe -d $Distro -- bash -lc $cmd; if($LASTEXITCODE -ne 0){throw "Build/test failed: $cmd"} }
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$baseBundle=Join-Path $root 'build/smoke/ChisBurner-storage-flush-smoke.49B-fullfile-control'
$base=Join-Path $baseBundle 'SD_ROOT'
$bundle=Join-Path $root 'build/smoke/ChisBurner-smoke.50-static-ab'
$zip="$bundle.zip"
if((Test-Path -LiteralPath $bundle) -or (Test-Path -LiteralPath $zip)){throw 'Preserve existing smoke50 artifact'}
$build=Join-Path $root 'build/storage-static50-app'
$osRel='Assets/chisburner/common/os.bin'
$appRel='Assets/chisburner/common/chisburnersmoke.elf'
$coreRel='Cores/ChisBurner.ChisBurner/core.json'
$dataRel='Cores/ChisBurner.ChisBurner/data.json'
$rbfRel='Cores/ChisBurner.ChisBurner/chis.rbf_r'
$instanceRel='Assets/chisburner/ChisBurner.ChisBurner/TF Static 50.json'
$fixtureRel='Assets/chisburner/ChisBurner.ChisBurner/static50.bin'
$omitted=@('Assets/chisburner/ChisBurner.ChisBurner/TF Snapshot 49.json',
 'Assets/chisburner/common/storage-seed46.bin','Saves/chisburner/common/storage-smoke.41.bin')
if((Sha "$baseBundle.zip") -ne '02fa3a274c6be5491fb17a1a24e214641af8d0e6a4c67c000831b66803be1a17'){throw 'Base ZIP mismatch'}
foreach($line in Get-Content -LiteralPath (Join-Path $baseBundle 'SHA256SUMS.txt')) {
 if($line -notmatch '^([0-9a-f]{64})  (.+)$'){throw 'Bad base manifest'}
 $h=$Matches[1];$p=$Matches[2];if((Sha (Join-Path $base $p)) -ne $h){throw "Base file mismatch $p"}
}
if((Sha (Join-Path $base $osRel)) -ne '17c8cae47bd24869e5fbaa01514ffcd0f5ffc77fd514f5305bd3eeb1a719f7fa'){throw 'Wrong base OS'}
if((Sha (Join-Path $base $rbfRel)) -ne 'b0c016f3c8956263aef31d8a934b366413a45885d0f7e72080bd605ae2197223'){throw 'Wrong base FPGA'}
# Confirm linked symbols came from the original matching smoke31 build.
$originalOs=Join-Path $root '.integration/openfpgaos/src/firmware/os/bld/pocket-output/os.bin'
if((Sha $originalOs) -ne 'e87121ba586e2a43eb99aa522e055e678a964199e1802968c11ef06d76dfebaa'){throw 'Symbol provenance OS mismatch'}
$wroot='/mnt/'+$root.Substring(0,1).ToLowerInvariant()+'/'+$root.Substring(3).Replace('\','/')
Wsl "cd '$wroot/app/storage_static_control' && make clean OPENFPGA_SDK='$OpenFpgaSdk' && make host-test all OPENFPGA_SDK='$OpenFpgaSdk'"
Wsl "cd '$wroot' && riscv64-unknown-elf-gcc -march=rv32imaf -mabi=ilp32f -Os -ffreestanding -fno-builtin -fno-pic -msmall-data-limit=0 -mno-relax -nostdlib -Wl,--no-relax,-T,tests/patches/static50.ld -I.integration/openfpgaos/src/firmware/api .integration/openfpgaos/src/firmware/os/targets/pocket/static_output_smoke50.c -o build/storage-static50-app/static50-patch.elf && riscv64-unknown-elf-objcopy -O binary -j .patch build/storage-static50-app/static50-patch.elf build/storage-static50-app/static50-patch.bin && c++ -std=c++17 -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined tests/test-static50-instructions.cpp -o build/storage-static50-app/instruction-test && build/storage-static50-app/instruction-test build/storage-static50-app/static50-patch.bin"
# Check function boundaries and globals in the preserved ELF independently of LD.
$nm=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && riscv64-unknown-elf-nm -n .integration/openfpgaos/src/firmware/os/bld/pocket-output/firmware.elf")
if($LASTEXITCODE -ne 0){throw 'Symbol check failed'}
foreach($s in @('1032f180 T svc_output_open.lto_priv.0','1032f400 T svc_output_write.lto_priv.0',
 '10328100 T of_file_async_busy','103297c0 T of_file_size','1038e530 b output_is_open',
 '1038e534 b output_open_size','1038e538 b output_open_slot')){if($nm -cnotcontains $s){throw "Symbol mismatch $s"}}
$old=[IO.File]::ReadAllBytes((Join-Path $base $osRel))
$null=Assert-ChisBurnerOsIntegrity -Bytes $old
$patch=[IO.File]::ReadAllBytes((Join-Path $build 'static50-patch.bin'))
if($patch.Length -gt 0x280 -or $patch.Length -lt 100){throw 'Patch outside bounded service region'}
$os=[byte[]]$old.Clone()
for($i=0xF180;$i -lt 0xF400;$i+=4){[Array]::Copy([byte[]](0x13,0,0,0),0,$os,$i,4)}
[Array]::Copy($patch,0,$os,0xF180,$patch.Length)
$os=Set-ChisBurnerOsCrc -Bytes $os
$changed=0
for($i=0;$i -lt $old.Length-4;++$i){if($os[$i] -ne $old[$i]){
 ++$changed;if($i -lt 0xF180 -or $i -ge 0xF400){throw 'Unexpected OS modification'}
}}
if($os.Length -ne 137480){throw 'OS layout changed'}
$sources=@('app/storage_static_control/main.c','app/storage_static_control/pattern.h',
 'app/storage_static_control/read512.h','app/storage_static_control/sequence.h',
 'app/storage_static_control/test_static50.c','app/storage_static_control/Makefile',
 '.integration/openfpgaos/src/firmware/api/of_static_output_test.h',
 '.integration/openfpgaos/src/firmware/os/targets/pocket/static_output_smoke50.c',
 'tests/patches/static50.ld','tests/test-static50-instructions.cpp','tests/build-static50.ps1',
 'app/storage_static_control/README_TEST.txt','app/storage_snapshot_forensics/snapshot.h')
$appSource=Get-Content -Raw (Join-Path $root 'app/storage_static_control/main.c')
if($appSource -match 'of_cart_|0x40000|\bfopen\s*\('){throw 'App MMIO/cart/cached IO not allowed'}
if(!$appSource.Contains('file_size("slot:4")')){throw 'Slot size namespace guard'}
if(![Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes((Join-Path $build 'app.elf'))).Contains('TF STATIC smoke.50')){throw 'Wrong ELF'}
$demo=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && build/storage-static50-app/pc-test --demo")
if($LASTEXITCODE -ne 0){throw 'UI demo failed'}
$demo=$demo | ForEach-Object {[regex]::Replace([string]$_,'\x1b\[[0-9;]*[A-Za-z]','')}
$rows=0;$pages=0;$maxRows=0;$width=0
foreach($line in $demo){
 if($line -eq ' TF STATIC smoke.50'){$maxRows=[Math]::Max($maxRows,$rows);$rows=0;++$pages}
 ++$rows;$width=[Math]::Max($width,$line.Length)
 if($line.Length -gt 40){throw "UI too wide: $line"}
}
$maxRows=[Math]::Max($maxRows,$rows)
if($maxRows -gt 18 -or $pages -ne 19){throw "UI shape $pages/$maxRows"}
# Create a new staging tree; no deletes/moves, no changes to previous packages.
$sd=Join-Path $bundle 'SD_ROOT'
$basePrefix=[IO.Path]::GetFullPath($base).TrimEnd('\')+'\'
$utf8=New-Object Text.UTF8Encoding($false)
New-Item -ItemType Directory -Path $bundle | Out-Null
foreach($f in Get-ChildItem -LiteralPath $base -Recurse -File){
 $rel=$f.FullName.Substring($basePrefix.Length).Replace('\','/')
 if($rel -in $omitted){continue}
 $dest=Join-Path $sd $rel
 New-Item -ItemType Directory -Force (Split-Path $dest -Parent) | Out-Null
 Copy-Item -LiteralPath $f.FullName -Destination $dest
}
[IO.File]::WriteAllBytes((Join-Path $sd $osRel),$os)
Copy-Item -LiteralPath (Join-Path $build 'app.elf') -Destination (Join-Path $sd $appRel)
$core=Get-Content -Raw (Join-Path $sd $coreRel) | ConvertFrom-Json
if([int]$core.core.framework.hardware.cartridge_adapter -ne -1){throw 'Cartridge must remain disabled'}
$core.core.metadata.version='0.1.0-smoke.50-static-ab'
$core.core.metadata.date_release='2026-09-03'
[IO.File]::WriteAllText((Join-Path $sd $coreRel),($core|ConvertTo-Json -Depth 25),$utf8)
$data=Get-Content -Raw (Join-Path $sd $dataRel) | ConvertFrom-Json
foreach($slot in $data.data.data_slots){
 if([int]$slot.id -in @(4,5,6)){$slot.extensions=@('bin')}
 if([int]$slot.id -eq 4){
  $slot.name='Static Test 50';$slot.parameters=2
  $slot | Add-Member -NotePropertyName filename -NotePropertyValue 'static50.bin' -Force
  $slot | Add-Member -NotePropertyName size_exact -NotePropertyValue 512 -Force
  if(!$slot.deferload -or $slot.nonvolatile -or $slot.address){throw 'Wrong static slot properties'}
 }
 if($slot.extensions.Count -gt 4){throw 'Nonconforming extensions'}
}
[IO.File]::WriteAllText((Join-Path $sd $dataRel),($data|ConvertTo-Json -Depth 25),$utf8)
$inst=Get-Content -Raw (Join-Path $base $omitted[0]) | ConvertFrom-Json
($inst.instance.data_slots | Where-Object id -eq 4).filename='static50.bin'
New-Item -ItemType Directory -Force (Split-Path (Join-Path $sd $instanceRel) -Parent) | Out-Null
[IO.File]::WriteAllText((Join-Path $sd $instanceRel),($inst|ConvertTo-Json -Depth 25),$utf8)
$fixture=New-Object byte[] 512
$magic=[Text.Encoding]::ASCII.GetBytes('CBRN50')
for($i=0;$i -lt 512;++$i){
 $fixture[$i]=if($i -lt 6){$magic[$i]}elseif($i -eq 6){0x33}elseif($i -eq 7){0xcc}else{
  [byte]((($i*73+($i -shr 3)*19) -bxor ($i -shr 8) -bxor 0x33) -band 255)
 }
}
[IO.File]::WriteAllBytes((Join-Path $sd $fixtureRel),$fixture)
foreach($f in Get-ChildItem -LiteralPath $base -Recurse -File){
 $rel=$f.FullName.Substring($basePrefix.Length).Replace('\','/')
 if($rel -in ($omitted+@($osRel,$appRel,$coreRel,$dataRel))){continue}
 if((Sha $f.FullName) -ne (Sha (Join-Path $sd $rel))){throw "Unexpected payload change $rel"}
}
Copy-Item -LiteralPath (Join-Path $root 'app/storage_static_control/README_TEST.txt') -Destination $bundle
[IO.File]::WriteAllLines((Join-Path $bundle 'UI_FIXTURES_SYNTHETIC.txt'),[string[]]$demo,$utf8)
[IO.File]::WriteAllLines((Join-Path $bundle 'SOFTWARE_SOURCE_SHA256.txt'),[string[]]@($sources | ForEach-Object {"$(Sha (Join-Path $root $_))  $_"}),$utf8)
$crc=Assert-ChisBurnerOsIntegrity -Bytes $os
$provenance=@"
smoke=50-static-ab
base=smoke49B (verified ZIP and all payloads)
os_sha256=$(Sha (Join-Path $sd $osRel))
os_length=$($os.Length)
os_internal_crc=$('{0:x8}' -f $crc.ComputedCrc)
os_changed_region=0xF180..0xF3FF and final4 CRC bytes ONLY
os_changed_bytes_excluding_crc=$changed
os_patch_compiled_length=$($patch.Length)
all_bootloader_loader_FPGA_bytes_unchanged=PASS
rbf_sha256=$(Sha (Join-Path $sd $rbfRel))
app_sha256=$(Sha (Join-Path $sd $appRel))
static_fixture_sha256=$(Sha (Join-Path $sd $fixtureRel))
fixture_size=512
host_OPEN_commands_from_static_test=0
host_resize_or_table_size_mutations_from_bind=0
sequence_A=bind RAM state;WRITE512;READ512;compare512;NO_FLUSH
sequence_B=bind RAM state;WRITE512;READ512;compare512;FLUSH only after match
boot_behavior=READ512 and classify INITIAL/A/B/UNKNOWN before any write
test_fault_injection=PASS ASan UBSan
compiled_patch_instruction_ABI_allowlist=PASS ASan UBSan
SDK_RV32_build=PASS
ui_pages=$pages
ui_max_columns=$width
ui_max_rows=$maxRows
host_APF_decode_or_electrical_simulation=NOT_PERFORMED
hardware_result=PENDING
"@
[IO.File]::WriteAllText((Join-Path $bundle 'BUILD_PROVENANCE.txt'),$provenance,$utf8)
$prefix=[IO.Path]::GetFullPath($sd).TrimEnd('\')+'\';$hashes=@{}
$sums=Get-ChildItem -LiteralPath $sd -Recurse -File | Sort-Object FullName | ForEach-Object {
 $rel=$_.FullName.Substring($prefix.Length).Replace('\','/');$h=Sha $_.FullName
 $hashes["SD_ROOT/$rel"]=$h;"$h  $rel"
}
[IO.File]::WriteAllLines((Join-Path $bundle 'SHA256SUMS.txt'),[string[]]$sums,$utf8)
Compress-Archive -Path (Join-Path $bundle '*') -DestinationPath $zip -CompressionLevel Optimal
$null=Assert-ChisBurnerZipOsIntegrity -ZipPath $zip
$archive=[IO.Compression.ZipFile]::OpenRead($zip)
try {
 foreach($name in $hashes.Keys){
  $e=$archive.GetEntry($name);if(!$e){throw "Missing ZIP entry $name"}
  $s=$e.Open();$hasher=[Security.Cryptography.SHA256]::Create()
  try{$h=[BitConverter]::ToString($hasher.ComputeHash($s)).Replace('-','').ToLowerInvariant();if($h -ne $hashes[$name]){throw "ZIP hash mismatch $name"}}
  finally{$s.Dispose();$hasher.Dispose()}
 }
}finally{$archive.Dispose()}
"PASS: $zip"
"ZIP SHA256: $(Sha $zip)"
"UI $pages pages max $width columns/$maxRows rows; OS CRC checked; $($hashes.Count) payload hashes checked"
