[CmdletBinding()]
param([string]$Distro='Ubuntu-24.04',
      [string]$OpenFpgaSdk='/home/lin13/work/ONS_FPGA/openfpgaSDK')
$ErrorActionPreference='Stop'
. (Join-Path $PSScriptRoot 'os-image-integrity.ps1')
function Sha([string]$p){(Get-FileHash -LiteralPath $p).Hash.ToLowerInvariant()}
function Wsl([string]$cmd){& wsl.exe -d $Distro -- bash -lc $cmd; if($LASTEXITCODE -ne 0){throw "Build/test failed: $cmd"}}
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$baseBundle=Join-Path $root 'build/smoke/ChisBurner-smoke.51-guarded'
$base=Join-Path $baseBundle 'SD_ROOT'
$bundle=Join-Path $root 'build/smoke/ChisBurner-smoke.52-stream1m'
$zip="$bundle.zip"
if((Test-Path -LiteralPath $bundle) -or (Test-Path -LiteralPath $zip)){throw 'Preserve existing smoke52 artifact'}
$build=Join-Path $root 'build/storage-stream52-app'
$osRel='Assets/chisburner/common/os.bin'
$appRel='Assets/chisburner/common/chisburner52.elf'
$iniRel='Assets/chisburner/common/chisburner52.ini'
$coreRel='Cores/ChisBurner.ChisBurner/core.json'
$dataRel='Cores/ChisBurner.ChisBurner/data.json'
$rbfRel='Cores/ChisBurner.ChisBurner/chis.rbf_r'
$instanceRel='Assets/chisburner/ChisBurner.ChisBurner/TF Stream 52.json'
$fixtureRel='Assets/chisburner/ChisBurner.ChisBurner/static52.bin'
$oldInstance='Assets/chisburner/ChisBurner.ChisBurner/TF Guard 51.json'
$omitted=@($oldInstance,'Assets/chisburner/common/chisburner51.elf','Assets/chisburner/common/chisburner51.ini',
 'Assets/chisburner/ChisBurner.ChisBurner/static51.bin')
if((Sha "$baseBundle.zip") -ne 'e4d743656dd41710a5ab8da2da7f8b358f92e523868be94f0513fd3ac7450a0e'){throw 'Base ZIP mismatch'}
foreach($line in Get-Content -LiteralPath (Join-Path $baseBundle 'SHA256SUMS.txt')) {
 if($line -notmatch '^([0-9a-f]{64})  (.+)$'){throw 'Bad base manifest'}
 $h=$Matches[1];$p=$Matches[2];if((Sha (Join-Path $base $p)) -ne $h){throw "Base file mismatch $p"}
}
if((Sha (Join-Path $base $osRel)) -ne '81c2169c1bd9648eb7ad19d7fe78408e1da19aa8143f578392f962dc24cf064b'){throw 'Wrong base OS'}
if((Sha (Join-Path $base $rbfRel)) -ne 'b0c016f3c8956263aef31d8a934b366413a45885d0f7e72080bd605ae2197223'){throw 'Wrong base FPGA'}
$originalOs=Join-Path $root '.integration/openfpgaos/src/firmware/os/bld/pocket-output/os.bin'
if((Sha $originalOs) -ne 'e87121ba586e2a43eb99aa522e055e678a964199e1802968c11ef06d76dfebaa'){throw 'Symbol provenance OS mismatch'}
# The exact accepted input gate is reused. Do not change its source for smoke52.
$reuse=@('app/storage_guarded51/guard.h','app/storage_guarded51/pattern.h')
$found=0
foreach($line in Get-Content -LiteralPath (Join-Path $baseBundle 'SOFTWARE_SOURCE_SHA256.txt')) {
 if($line -match '^([0-9a-f]{64})  (.+)$') {
  $h=$Matches[1];$p=$Matches[2]
  if($p -in $reuse){++$found;if((Sha (Join-Path $root $p)) -ne $h){throw "Reused source drift $p"}}
 }
}
if($found -ne $reuse.Count){throw 'Missing reused source hashes'}
$wroot='/mnt/'+$root.Substring(0,1).ToLowerInvariant()+'/'+$root.Substring(3).Replace('\','/')
# sdk.mk does not track all transitive headers. Always recompile main.o.
Wsl "cd '$wroot/app/storage_stream52' && make host-test all --what-if=main.c OPENFPGA_SDK='$OpenFpgaSdk'"
Wsl "cd '$wroot' && cc -DOF_PC -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined -I.integration/openfpgaos/src/firmware/api app/storage_guarded51/test_guard51.c -o build/storage-stream52-app/guard51-regression && build/storage-stream52-app/guard51-regression"
Wsl "cd '$wroot' && riscv64-unknown-elf-gcc -march=rv32imaf -mabi=ilp32f -Os -ffreestanding -fno-builtin -fno-pic -msmall-data-limit=0 -mno-relax -nostdlib -Wl,--no-relax,-T,tests/patches/static52.ld -I.integration/openfpgaos/src/firmware/api .integration/openfpgaos/src/firmware/os/targets/pocket/static_output_smoke52.c -o build/storage-stream52-app/static52-patch.elf && riscv64-unknown-elf-objcopy -O binary -j .patch build/storage-stream52-app/static52-patch.elf build/storage-stream52-app/static52-patch.bin && c++ -std=c++17 -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined tests/test-static52-instructions.cpp -o build/storage-stream52-app/instruction-test && build/storage-stream52-app/instruction-test build/storage-stream52-app/static52-patch.bin"
$nm=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && riscv64-unknown-elf-nm -n .integration/openfpgaos/src/firmware/os/bld/pocket-output/firmware.elf")
if($LASTEXITCODE -ne 0){throw 'Symbol check failed'}
foreach($s in @('1032f180 T svc_output_open.lto_priv.0','1032f400 T svc_output_write.lto_priv.0',
 '10328100 T of_file_async_busy','103297c0 T of_file_size','1038e530 b output_is_open',
 '1038e534 b output_open_size','1038e538 b output_open_slot')){if($nm -cnotcontains $s){throw "Symbol mismatch $s"}}
$old=[IO.File]::ReadAllBytes((Join-Path $base $osRel))
$null=Assert-ChisBurnerOsIntegrity -Bytes $old
$patch=[IO.File]::ReadAllBytes((Join-Path $build 'static52-patch.bin'))
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
$crc=Assert-ChisBurnerOsIntegrity -Bytes $os
$runtime=@('app/storage_stream52/main.c','app/storage_stream52/pattern.h','app/storage_stream52/gate.h',
 'app/storage_stream52/stream.h','app/storage_stream52/read_block.h',
 '.integration/openfpgaos/src/firmware/api/of_static_output52.h')
foreach($p in $runtime) {
 $src=Get-Content -Raw (Join-Path $root $p)
 if($src -match 'of_cart_|0x40000|\bfopen\s*\(|\bof_output_flush\s*\('){throw "Forbidden runtime access $p"}
}
$appSource=Get-Content -Raw (Join-Path $root $runtime[0])
if(!$appSource.Contains('file_size("slot:4")')){throw 'Slot size namespace guard'}
if(!$appSource.Contains('_of_input_p0.buttons') -or $appSource.Contains('of_btn_pressed(')){throw 'Raw input gate guard'}
$elf=[IO.File]::ReadAllBytes((Join-Path $build 'app.elf'))
if(![Text.Encoding]::ASCII.GetString($elf).Contains('TF STREAM smoke.52')){throw 'Wrong ELF'}
if($elf.Length -gt 262144 -or $elf[0] -ne 0x7f -or $elf[4] -ne 1 -or $elf[5] -ne 1 -or [BitConverter]::ToUInt16($elf,18) -ne 243){throw 'Invalid RV32 ELF shape'}
# Source has no FLUSH call; also ensure its inline service call is absent from the ELF symbol table.
$appNm=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && riscv64-unknown-elf-nm build/storage-stream52-app/app.elf")
if($LASTEXITCODE -ne 0 -or ($appNm -match 'of_output_flush|of_cart_')){throw 'Forbidden app symbol'}
$demo=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && build/storage-stream52-app/pc-test --demo")
if($LASTEXITCODE -ne 0){throw 'UI demo failed'}
$demo=$demo | ForEach-Object {[regex]::Replace([string]$_,'\x1b\[[0-9;]*[A-Za-z]','')}
$rows=0;$pages=0;$maxRows=0;$width=0
foreach($line in $demo) {
 if($line -eq ' TF STREAM smoke.52'){$maxRows=[Math]::Max($maxRows,$rows);$rows=0;++$pages}
 ++$rows;$width=[Math]::Max($width,$line.Length)
 if($line.Length -gt 40){throw "UI too wide: $line"}
}
$maxRows=[Math]::Max($maxRows,$rows)
if($maxRows -gt 18 -or $pages -ne 25){throw "UI shape $pages/$maxRows"}
# Build artifacts below; no edits/deletes of previous packages or SD cards.
$sd=Join-Path $bundle 'SD_ROOT'
$basePrefix=[IO.Path]::GetFullPath($base).TrimEnd('\')+'\'
$utf8=New-Object Text.UTF8Encoding($false)
New-Item -ItemType Directory -Path $bundle | Out-Null
foreach($f in Get-ChildItem -LiteralPath $base -Recurse -File) {
 $rel=$f.FullName.Substring($basePrefix.Length).Replace('\','/')
 if($rel -in $omitted){continue}
 $dest=Join-Path $sd $rel
 New-Item -ItemType Directory -Force (Split-Path $dest -Parent) | Out-Null
 Copy-Item -LiteralPath $f.FullName -Destination $dest
}
[IO.File]::WriteAllBytes((Join-Path $sd $osRel),$os)
Copy-Item -LiteralPath (Join-Path $build 'app.elf') -Destination (Join-Path $sd $appRel)
[IO.File]::WriteAllText((Join-Path $sd $iniRel),"[os]`nELF=chisburner52.elf`nARGS=`n",$utf8)
$core=Get-Content -Raw (Join-Path $sd $coreRel) | ConvertFrom-Json
if([int]$core.core.framework.hardware.cartridge_adapter -ne -1){throw 'Cartridge must remain disabled'}
$core.core.metadata.version='0.1.0-smoke.52-stream1m'
$core.core.metadata.date_release='2026-09-03'
[IO.File]::WriteAllText((Join-Path $sd $coreRel),($core|ConvertTo-Json -Depth 25),$utf8)
$data=Get-Content -Raw (Join-Path $sd $dataRel) | ConvertFrom-Json
foreach($slot in $data.data.data_slots) {
 switch([int]$slot.id) {
  2 {$slot.filename='chisburner52.ini'}
  3 {$slot.filename='chisburner52.elf'}
  4 {$slot.name='Stream Test 52';$slot.filename='static52.bin';$slot.size_exact=1048576
     if([int]$slot.parameters -ne 2 -or !$slot.deferload -or $slot.nonvolatile -or $slot.address){throw 'Wrong slot4 properties'}}
 }
 if($slot.extensions.Count -gt 4){throw 'Nonconforming extensions'}
}
[IO.File]::WriteAllText((Join-Path $sd $dataRel),($data|ConvertTo-Json -Depth 25),$utf8)
$inst=Get-Content -Raw (Join-Path $base $oldInstance) | ConvertFrom-Json
if(($inst.instance.data_slots.id -join ',') -ne '1,2,3,4'){throw 'Instance slots changed'}
$mapping=@{1='os.bin';2='chisburner52.ini';3='chisburner52.elf';4='static52.bin'}
foreach($slot in $inst.instance.data_slots){$slot.filename=$mapping[[int]$slot.id]}
New-Item -ItemType Directory -Force (Split-Path (Join-Path $sd $instanceRel) -Parent) | Out-Null
[IO.File]::WriteAllText((Join-Path $sd $instanceRel),($inst|ConvertTo-Json -Depth 25),$utf8)
$wfixture="$wroot/build/smoke/ChisBurner-smoke.52-stream1m/SD_ROOT/$fixtureRel"
Wsl "cd '$wroot' && build/storage-stream52-app/stream-test --make-fixture '$wfixture' && build/storage-stream52-app/stream-test --check-fixture '$wfixture'"
$fixture=[IO.File]::ReadAllBytes((Join-Path $sd $fixtureRel))
if($fixture.Length -ne 1048576 -or [ChisBurner.Build.OsCrc32]::Compute($fixture,1048576) -ne 0x502b18cd){throw 'Independent fixture CRC mismatch'}
# Verify instance defaults, resolved file locations and actual file size.
$inst=Get-Content -Raw (Join-Path $sd $instanceRel) | ConvertFrom-Json
foreach($slot in $inst.instance.data_slots) {
 $default=$data.data.data_slots | Where-Object id -eq $slot.id
 if($default.filename -cne $slot.filename){throw 'Instance/default mismatch'}
 $dir=if([int]$slot.id -eq 4){'Assets/chisburner/ChisBurner.ChisBurner'}else{'Assets/chisburner/common'}
 $resolved=Join-Path (Join-Path $sd $dir) $slot.filename
 if(!(Test-Path -LiteralPath $resolved)){throw 'Missing bound payload'}
 if([int]$slot.id -eq 4 -and (Get-Item -LiteralPath $resolved).Length -ne $default.size_exact){throw 'Fixture size/default mismatch'}
}
foreach($f in Get-ChildItem -LiteralPath $base -Recurse -File) {
 $rel=$f.FullName.Substring($basePrefix.Length).Replace('\','/')
 if($rel -in ($omitted+@($coreRel,$dataRel,$osRel))){continue}
 if((Sha $f.FullName) -ne (Sha (Join-Path $sd $rel))){throw "Unexpected payload change $rel"}
}
foreach($rel in $omitted){if(Test-Path -LiteralPath (Join-Path $sd $rel)){throw "Old payload leaked $rel"}}
if(Test-Path -LiteralPath (Join-Path $sd 'Saves')){throw 'No user save payloads allowed'}
Copy-Item -LiteralPath (Join-Path $root 'app/storage_stream52/README_TEST.txt') -Destination $bundle
[IO.File]::WriteAllLines((Join-Path $bundle 'UI_FIXTURES_SYNTHETIC.txt'),[string[]]$demo,$utf8)
$sources=$runtime+$reuse+@('app/storage_stream52/test_stream52.c','app/storage_stream52/Makefile',
 'app/storage_stream52/README_TEST.txt','tests/build-stream52.ps1','tests/os-image-integrity.ps1',
 'tests/patches/static52.ld','tests/test-static52-instructions.cpp',
 '.integration/openfpgaos/src/firmware/os/targets/pocket/static_output_smoke52.c',
 '.integration/openfpgaos/src/firmware/api/of_output.h',
 '.integration/openfpgaos/src/firmware/api/of_file.h',
 '.integration/openfpgaos/src/firmware/api/of_input.h')
[IO.File]::WriteAllLines((Join-Path $bundle 'SOFTWARE_SOURCE_SHA256.txt'),[string[]]@($sources | ForEach-Object {"$(Sha (Join-Path $root $_))  $_"}),$utf8)
$provenance=@"
smoke=52-stream1m
base=smoke51 (verified ZIP and all payloads)
loader_FPGA_bytes_unchanged=PASS
os_sha256=$(Sha (Join-Path $sd $osRel))
os_length=$($crc.Length)
os_internal_crc=$('{0:x8}' -f $crc.ComputedCrc)
os_changed_region=0xF180..0xF3FF and final4 CRC bytes ONLY
os_changed_bytes_excluding_crc=$changed
os_patch_compiled_length=$($patch.Length)
os_patch_sha256=$(Sha (Join-Path $build 'static52-patch.bin'))
rbf_sha256=$(Sha (Join-Path $sd $rbfRel))
app_sha256=$(Sha (Join-Path $sd $appRel))
static_fixture_sha256=$(Sha (Join-Path $sd $fixtureRel))
fixture_size=1048576
block_size=4096
blocks=256
initial_crc32=502b18cd
written_crc32=cc3fa633
entry=TF Stream 52.json
unique_app=chisburner52.elf
unique_config=chisburner52.ini
unique_fixture=static52.bin
startup=full 1MiB READ+byte compare+CRC; ZERO bind/write/flush
gate=release500;L+R2000;release500;A;release500;START1000;10sec window;one shot
B_button=CANCEL;no flush mode
sequence=static bind;256 WRITE4096;256 READ4096;compare all bytes+CRC;NO_FLUSH
written_or_invalid_boot_content=READ_ONLY;no rewrite
host_OPEN_resize_table_size_writes=0
native_fault_gate_boundary_DMA_tests=PASS ASan UBSan
smoke51_gate_regression=PASS ASan UBSan
linked_RV32_service_ABI_instruction_allowlist=PASS ASan UBSan runner
fixture_checked_by_app_C_pattern_code_and_independent_CSharp_CRC=PASS
SDK_RV32_build=PASS
ui_synthetic_pages=$pages
ui_max_columns=$width
ui_max_rows=$maxRows
host_APF_decode_or_electrical_simulation=NOT_PERFORMED
flush_issue=NOT_CLAIMED_FIXED;not called, not converted to success
hardware_result=PENDING
persistence_test=normal quit then power off/on then full read;NOT sudden power loss test
"@
[IO.File]::WriteAllText((Join-Path $bundle 'BUILD_PROVENANCE.txt'),$provenance,$utf8)
$prefix=[IO.Path]::GetFullPath($sd).TrimEnd('\')+'\';$hashes=@{}
$sums=Get-ChildItem -LiteralPath $sd -Recurse -File | Sort-Object FullName | ForEach-Object {
 $rel=$_.FullName.Substring($prefix.Length).Replace('\','/');$h=Sha $_.FullName
 $hashes["SD_ROOT/$rel"]=$h;"$h  $rel"
}
if($hashes.Count -ne 16){throw "Unexpected payload count $($hashes.Count)"}
[IO.File]::WriteAllLines((Join-Path $bundle 'SHA256SUMS.txt'),[string[]]$sums,$utf8)
Compress-Archive -Path (Join-Path $bundle '*') -DestinationPath $zip -CompressionLevel Optimal
$null=Assert-ChisBurnerZipOsIntegrity -ZipPath $zip
$archive=[IO.Compression.ZipFile]::OpenRead($zip)
try {
 $payloads=@($archive.Entries | Where-Object { $_.FullName.StartsWith('SD_ROOT/') -and $_.Length -gt 0 })
 if($payloads.Count -ne $hashes.Count){throw 'ZIP payload count mismatch'}
 foreach($name in $hashes.Keys) {
  $e=$archive.GetEntry($name);if(!$e){throw "Missing ZIP entry $name"}
  $s=$e.Open();$hasher=[Security.Cryptography.SHA256]::Create()
  try{$h=[BitConverter]::ToString($hasher.ComputeHash($s)).Replace('-','').ToLowerInvariant();if($h -ne $hashes[$name]){throw "ZIP hash mismatch $name"}}
  finally{$s.Dispose();$hasher.Dispose()}
 }
}finally{$archive.Dispose()}
"PASS: $zip"
"ZIP SHA256: $(Sha $zip)"
"UI $pages pages max $width columns/$maxRows rows; OS bounded patch+CRC; $($hashes.Count) payload hashes checked"
