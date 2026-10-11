[CmdletBinding()]
param([string]$Distro='Ubuntu-24.04',
      [string]$OpenFpgaSdk='/home/lin13/work/ONS_FPGA/openfpgaSDK')
$ErrorActionPreference='Stop'
. (Join-Path $PSScriptRoot 'os-image-integrity.ps1')
function Sha([string]$p){(Get-FileHash -LiteralPath $p).Hash.ToLowerInvariant()}
function Wsl([string]$cmd){& wsl.exe -d $Distro -- bash -lc $cmd; if($LASTEXITCODE -ne 0){throw "Build/test failed: $cmd"}}
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$baseBundle=Join-Path $root 'build/smoke/ChisBurner-smoke.50-static-ab'
$base=Join-Path $baseBundle 'SD_ROOT'
$bundle=Join-Path $root 'build/smoke/ChisBurner-smoke.51-guarded'
$zip="$bundle.zip"
if((Test-Path -LiteralPath $bundle) -or (Test-Path -LiteralPath $zip)){throw 'Preserve existing smoke51 artifact'}
$build=Join-Path $root 'build/storage-guarded51-app'
$osRel='Assets/chisburner/common/os.bin'
$appRel='Assets/chisburner/common/chisburner51.elf'
$iniRel='Assets/chisburner/common/chisburner51.ini'
$coreRel='Cores/ChisBurner.ChisBurner/core.json'
$dataRel='Cores/ChisBurner.ChisBurner/data.json'
$rbfRel='Cores/ChisBurner.ChisBurner/chis.rbf_r'
$instanceRel='Assets/chisburner/ChisBurner.ChisBurner/TF Guard 51.json'
$fixtureRel='Assets/chisburner/ChisBurner.ChisBurner/static51.bin'
$oldInstance='Assets/chisburner/ChisBurner.ChisBurner/TF Static 50.json'
$omitted=@($oldInstance,'Assets/chisburner/common/chisburnersmoke.elf','Assets/chisburner/common/chisburnersmoke.ini',
 'Assets/chisburner/ChisBurner.ChisBurner/static50.bin','Saves/chisburner/common/storage-smoke.31.bin')
if((Sha "$baseBundle.zip") -ne '7d02d6aa87a8efe0822fba198bde615ffee7989b16adf1a8735748e69e5a1fa7'){throw 'Base ZIP mismatch'}
foreach($line in Get-Content -LiteralPath (Join-Path $baseBundle 'SHA256SUMS.txt')) {
 if($line -notmatch '^([0-9a-f]{64})  (.+)$'){throw 'Bad base manifest'}
 $h=$Matches[1];$p=$Matches[2];if((Sha (Join-Path $base $p)) -ne $h){throw "Base file mismatch $p"}
}
$expectedOs='81c2169c1bd9648eb7ad19d7fe78408e1da19aa8143f578392f962dc24cf064b'
$expectedRbf='b0c016f3c8956263aef31d8a934b366413a45885d0f7e72080bd605ae2197223'
if((Sha (Join-Path $base $osRel)) -ne $expectedOs){throw 'Wrong base OS'}
if((Sha (Join-Path $base $rbfRel)) -ne $expectedRbf){throw 'Wrong base FPGA'}
$crc=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes((Join-Path $base $osRel)))
# Reused read/recorder/service headers must match the smoke50 source manifest.
$reuse=@('app/storage_static_control/read512.h','app/storage_snapshot_forensics/snapshot.h',
 '.integration/openfpgaos/src/firmware/api/of_static_output_test.h')
$found=0
foreach($line in Get-Content -LiteralPath (Join-Path $baseBundle 'SOFTWARE_SOURCE_SHA256.txt')) {
 if($line -match '^([0-9a-f]{64})  (.+)$') {
  $h=$Matches[1];$p=$Matches[2]
  if($p -in $reuse){++$found;if((Sha (Join-Path $root $p)) -ne $h){throw "Reused source drift $p"}}
 }
}
if($found -ne $reuse.Count){throw 'Missing reused source hashes'}
$wroot='/mnt/'+$root.Substring(0,1).ToLowerInvariant()+'/'+$root.Substring(3).Replace('\','/')
# Use a fresh compiler invocation for main.o on every packaging attempt,
# because sdk.mk does not track every transitive local header as a dependency.
Wsl "cd '$wroot/app/storage_guarded51' && make host-test all --what-if=main.c OPENFPGA_SDK='$OpenFpgaSdk'"
Wsl "cd '$wroot' && cc -DOF_PC -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined -I.integration/openfpgaos/src/firmware/api app/storage_static_control/test_static50.c -o build/storage-guarded51-app/reused-read-test && build/storage-guarded51-app/reused-read-test"
$appSource=Get-Content -Raw (Join-Path $root 'app/storage_guarded51/main.c')
if($appSource -match 'of_cart_|0x40000|\bfopen\s*\('){throw 'App MMIO/cart/cached IO not allowed'}
if(!$appSource.Contains('file_size("slot:4")')){throw 'Slot size namespace guard'}
if(!$appSource.Contains('_of_input_p0.buttons') -or $appSource.Contains('of_btn_pressed(')){throw 'Raw input gate guard'}
$elf=[IO.File]::ReadAllBytes((Join-Path $build 'app.elf'))
if(![Text.Encoding]::ASCII.GetString($elf).Contains('TF GUARD smoke.51')){throw 'Wrong ELF'}
if($elf.Length -gt 262144 -or $elf[0] -ne 0x7f -or $elf[4] -ne 1 -or $elf[5] -ne 1){throw 'Invalid ELF shape'}
$demo=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && build/storage-guarded51-app/pc-test --demo")
if($LASTEXITCODE -ne 0){throw 'UI demo failed'}
$demo=$demo | ForEach-Object {[regex]::Replace([string]$_,'\x1b\[[0-9;]*[A-Za-z]','')}
$rows=0;$pages=0;$maxRows=0;$width=0
foreach($line in $demo) {
 if($line -eq ' TF GUARD smoke.51'){$maxRows=[Math]::Max($maxRows,$rows);$rows=0;++$pages}
 ++$rows;$width=[Math]::Max($width,$line.Length)
 if($line.Length -gt 40){throw "UI too wide: $line"}
}
$maxRows=[Math]::Max($maxRows,$rows)
if($maxRows -gt 18 -or $pages -ne 32){throw "UI shape $pages/$maxRows"}
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
Copy-Item -LiteralPath (Join-Path $build 'app.elf') -Destination (Join-Path $sd $appRel)
[IO.File]::WriteAllText((Join-Path $sd $iniRel),"[os]`nELF=chisburner51.elf`nARGS=`n",$utf8)
$core=Get-Content -Raw (Join-Path $sd $coreRel) | ConvertFrom-Json
if([int]$core.core.framework.hardware.cartridge_adapter -ne -1){throw 'Cartridge must remain disabled'}
$core.core.metadata.version='0.1.0-smoke.51-guarded'
$core.core.metadata.date_release='2026-09-03'
[IO.File]::WriteAllText((Join-Path $sd $coreRel),($core|ConvertTo-Json -Depth 25),$utf8)
$data=Get-Content -Raw (Join-Path $sd $dataRel) | ConvertFrom-Json
foreach($slot in $data.data.data_slots) {
 switch([int]$slot.id) {
  2 {$slot.filename='chisburner51.ini'}
  3 {$slot.filename='chisburner51.elf'}
  4 {$slot.name='Guarded Test 51';$slot.filename='static51.bin'
     if([int]$slot.parameters -ne 2 -or !$slot.deferload -or $slot.nonvolatile -or $slot.address -or [int]$slot.size_exact -ne 512){throw 'Wrong slot4 properties'}}
 }
 if($slot.extensions.Count -gt 4){throw 'Nonconforming extensions'}
}
[IO.File]::WriteAllText((Join-Path $sd $dataRel),($data|ConvertTo-Json -Depth 25),$utf8)
$inst=Get-Content -Raw (Join-Path $base $oldInstance) | ConvertFrom-Json
if(($inst.instance.data_slots.id -join ',') -ne '1,2,3,4'){throw 'Instance slots changed'}
$mapping=@{1='os.bin';2='chisburner51.ini';3='chisburner51.elf';4='static51.bin'}
foreach($slot in $inst.instance.data_slots){$slot.filename=$mapping[[int]$slot.id]}
New-Item -ItemType Directory -Force (Split-Path (Join-Path $sd $instanceRel) -Parent) | Out-Null
[IO.File]::WriteAllText((Join-Path $sd $instanceRel),($inst|ConvertTo-Json -Depth 25),$utf8)
$fixture=New-Object byte[] 512
$magic=[Text.Encoding]::ASCII.GetBytes('CBRN51')
for($i=0;$i -lt 512;++$i) {
 $fixture[$i]=if($i -lt 6){$magic[$i]}elseif($i -eq 6){0x3c}elseif($i -eq 7){0xc3}else{
  [byte]((($i*73+($i -shr 3)*19) -bxor ($i -shr 8) -bxor 0x3c -bxor 0x51) -band 255)
 }
}
if([ChisBurner.Build.OsCrc32]::Compute($fixture,512) -ne 0x3b942626){throw 'Fixture CRC mismatch'}
[IO.File]::WriteAllBytes((Join-Path $sd $fixtureRel),$fixture)
$wfixture="$wroot/build/smoke/ChisBurner-smoke.51-guarded/SD_ROOT/$fixtureRel"
Wsl "cd '$wroot' && build/storage-guarded51-app/guard-test --check-fixture '$wfixture'"
# Verify every instance filename against both defaults and the actual payload.
$inst=Get-Content -Raw (Join-Path $sd $instanceRel) | ConvertFrom-Json
foreach($slot in $inst.instance.data_slots) {
 $default=$data.data.data_slots | Where-Object id -eq $slot.id
 if($default.filename -cne $slot.filename){throw 'Instance/default mismatch'}
 $dir=if([int]$slot.id -eq 4){'Assets/chisburner/ChisBurner.ChisBurner'}else{'Assets/chisburner/common'}
 if(!(Test-Path -LiteralPath (Join-Path (Join-Path $sd $dir) $slot.filename))){throw 'Missing bound payload'}
}
foreach($f in Get-ChildItem -LiteralPath $base -Recurse -File) {
 $rel=$f.FullName.Substring($basePrefix.Length).Replace('\','/')
 if($rel -in ($omitted+@($coreRel,$dataRel))){continue}
 if((Sha $f.FullName) -ne (Sha (Join-Path $sd $rel))){throw "Unexpected payload change $rel"}
}
foreach($rel in $omitted){if(Test-Path -LiteralPath (Join-Path $sd $rel)){throw "Old payload leaked $rel"}}
Copy-Item -LiteralPath (Join-Path $root 'app/storage_guarded51/README_TEST.txt') -Destination $bundle
[IO.File]::WriteAllLines((Join-Path $bundle 'UI_FIXTURES_SYNTHETIC.txt'),[string[]]$demo,$utf8)
$sources=@('app/storage_guarded51/main.c','app/storage_guarded51/pattern.h','app/storage_guarded51/guard.h',
 'app/storage_guarded51/sequence.h','app/storage_guarded51/test_guard51.c','app/storage_guarded51/Makefile',
 'app/storage_guarded51/README_TEST.txt','tests/build-guarded51.ps1','tests/os-image-integrity.ps1')+$reuse
[IO.File]::WriteAllLines((Join-Path $bundle 'SOFTWARE_SOURCE_SHA256.txt'),[string[]]@($sources | ForEach-Object {"$(Sha (Join-Path $root $_))  $_"}),$utf8)
$provenance=@"
smoke=51-guarded
base=smoke50 (verified ZIP and all payloads)
OS_FPGA_loader_bytes_unchanged=PASS
os_sha256=$(Sha (Join-Path $sd $osRel))
os_length=$($crc.Length)
os_internal_crc=$('{0:x8}' -f $crc.ComputedCrc)
rbf_sha256=$(Sha (Join-Path $sd $rbfRel))
app_sha256=$(Sha (Join-Path $sd $appRel))
static_fixture_sha256=$(Sha (Join-Path $sd $fixtureRel))
fixture_size=512
fixture_crc32=3b942626
entry=TF Guard 51.json
unique_app=chisburner51.elf
unique_config=chisburner51.ini
unique_fixture=static51.bin
startup=two successful READ512 with repeat and complete pattern checks; ZERO bind/write/flush
gate=release500;L+R2000;release500;A or B;release500;START1000;10sec window;one shot
sequence_A=static bind;WRITE512;READ512;compare512;NO_FLUSH
sequence_B=static bind;WRITE512;READ512;compare512;FLUSH only after match
host_OPEN_resize_table_size_writes=0
native_gate_fault_tests=PASS ASan UBSan
original_read512_regression=PASS ASan UBSan
fixture_checked_by_app_C_pattern_code=PASS
SDK_RV32_build=PASS
ui_synthetic_pages=$pages
ui_max_columns=$width
ui_max_rows=$maxRows
host_APF_decode_or_electrical_simulation=NOT_PERFORMED
smoke50_unexpected_A_root_cause=UNCONFIRMED
flush_issue=NOT_CLAIMED_FIXED
hardware_result=PENDING
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
"UI $pages pages max $width columns/$maxRows rows; OS CRC checked; $($hashes.Count) payload hashes checked"
