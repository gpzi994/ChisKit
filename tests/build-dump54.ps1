[CmdletBinding()]
param([string]$Distro='Ubuntu-24.04',[string]$OpenFpgaSdk='/home/lin13/work/ONS_FPGA/openfpgaSDK')
$ErrorActionPreference='Stop'
. (Join-Path $PSScriptRoot 'os-image-integrity.ps1')
function Sha([string]$p){(Get-FileHash -LiteralPath $p).Hash.ToLowerInvariant()}
function Wsl([string]$cmd){& wsl.exe -d $Distro -- bash -lc $cmd;if($LASTEXITCODE -ne 0){throw "Build/test failed: $cmd"}}
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$wroot='/mnt/'+$root.Substring(0,1).ToLowerInvariant()+'/'+$root.Substring(3).Replace('\','/')
$baseBundle=Join-Path $root 'build/smoke/ChisBurner-smoke.53-gba-dump'
$base=Join-Path $baseBundle 'SD_ROOT'
$bundle=Join-Path $root 'build/smoke/ChisBurner-smoke.54-gba-dump'
$zip="$bundle.zip"
if((Test-Path -LiteralPath $bundle) -or (Test-Path -LiteralPath $zip)){throw 'Preserve existing smoke54 package'}
$build=Join-Path $root 'build/dump54-app'
$fpga=Join-Path $root '.integration/openfpgaos/src/fpga/targets/pocket/bld/chisburner-dump53-s3-win'
$utf8=New-Object Text.UTF8Encoding($false)
$osRel='Assets/chisburner/common/os.bin'
$coreRel='Cores/ChisBurner.ChisBurner/core.json'
$dataRel='Cores/ChisBurner.ChisBurner/data.json'
$rbfRel='Cores/ChisBurner.ChisBurner/chis.rbf_r'
$appRel='Assets/chisburner/common/chisburner54.elf'
$iniRel='Assets/chisburner/common/chisburner54.ini'
$fixtureRel='Assets/chisburner/ChisBurner.ChisBurner/dump54.gba'
$omitted=@('Assets/chisburner/common/chisburner53.elf','Assets/chisburner/common/chisburner53.ini',
 'Assets/chisburner/ChisBurner.ChisBurner/dump53.gba')
if((Sha "$baseBundle.zip") -ne '92baac1bb31012936848552c4bf0c68cd2e3021f5d6e88e947c25189fd404c57'){throw 'Wrong base ZIP'}
foreach($line in Get-Content (Join-Path $baseBundle 'SHA256SUMS.txt')){
 if($line -notmatch '^([0-9a-f]{64})  (.+)$'){throw 'Bad base manifest'}
 $h=$Matches[1];$p=$Matches[2];if((Sha (Join-Path $base $p)) -ne $h){throw "Base changed: $p"}
}
$rom='C:/Users/lin13/Desktop/game/逆转裁判.gba';$sav='C:/Users/lin13/Desktop/game/逆转裁判.sav'
$romHash='0d0945fff2edd4d4ae627ed003ebb0050937ee689d606ccff89c4ff2c2638571'
$savHash='30cd41ec01aaf4f83919cd0b86d8ce0939b0c3582965496d8db31940cc1099ae'
if((Sha $rom) -ne $romHash -or (Sha $sav) -ne $savHash){throw 'Reference source changed'}
if((Get-Item -LiteralPath $rom).Length -ne 8705280 -or (Get-Item -LiteralPath $sav).Length -ne 32768){throw 'Reference sizes changed'}
# Boundaries preserved through SDK rebuild, fixed-layout OS extension, FPGA MIF update.
Wsl "cd '$wroot/app/gba_dump54' && make host-test all --what-if=main.c OPENFPGA_SDK='$OpenFpgaSdk'"
Wsl "cd '$wroot' && riscv64-unknown-elf-gcc -march=rv32imaf -mabi=ilp32f -Os -ffreestanding -fno-builtin -fno-pic -msmall-data-limit=0 -mno-relax -nostdlib -Wl,--no-relax,-T,tests/patches/dump54.ld -I.integration/openfpgaos/src/firmware/api .integration/openfpgaos/src/firmware/os/targets/pocket/dump54_services.c tests/patches/dump54_entry.S -o build/dump54-app/services.elf && riscv64-unknown-elf-objcopy -O binary build/dump54-app/services.elf build/dump54-app/services.bin && c++ -std=c++17 -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined tests/test-dump54-instructions.cpp -o build/dump54-app/instruction-test && build/dump54-app/instruction-test build/dump54-app/services.bin"
Wsl "cd '$wroot' && cc -DOF_PC -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined -I.integration/openfpgaos/src/firmware/api tests/test-dump54-services.c -o build/dump54-app/services-test && build/dump54-app/services-test && cc -DOF_PC -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined -I.integration/openfpgaos/src/firmware/api app/storage_guarded51/test_guard51.c -o build/dump54-app/guard51-test && build/dump54-app/guard51-test"
Wsl "cd '$wroot' && verilator --cc --exe --build -j 4 -Wno-fatal --top-module chisburner_cart_gba_readonly -Mdir '$wroot/build/dump54-rtl' -GREADS_ENABLED=1 -GADDRESS_SETUP_CYCLES=12 -GSELECT_SETUP_CYCLES=6 -GTURNAROUND_CYCLES=3 -GDATA_WAIT_CYCLES=18 -GRELEASE_CYCLES=6 -GMAX_READ_COMMANDS=272045 -GREQUIRED_READ_BYTES=32 -GREAD_ADDRESS_MASK=31 -GREAD_WINDOW_BYTES=8705280 .integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gba_readonly.v '$wroot/tests/test-dump53-rtl.cpp' > build/dump54-app/rtl-build.log 2>&1 && build/dump54-rtl/Vchisburner_cart_gba_readonly"
$reuse=@('app/storage_guarded51/guard.h','app/storage_guarded51/pattern.h','app/storage_stream52/gate.h','app/gba_dump53/md5.h')
$found=0
foreach($line in Get-Content (Join-Path $baseBundle 'SOFTWARE_SOURCE_SHA256.txt')){
 if($line -match '^([0-9a-f]{64})  (.+)$'){$h=$Matches[1];$p=$Matches[2];if($p -in $reuse){++$found;if((Sha (Join-Path $root $p)) -ne $h){throw "Reused source drift $p"}}}
}
if($found -ne 4){throw 'Missing gate source provenance'}
foreach($line in Get-Content (Join-Path $fpga 'SOURCE_SHA256.txt')){
 if($line -notmatch '^([0-9a-f]{64})  (.+)$'){throw 'Bad FPGA source manifest'}
 $h=$Matches[1];$p=$Matches[2];if((Sha $p) -ne $h){throw "FPGA source changed $p"}
}
$qsf=Get-Content -Raw (Join-Path $fpga 'ap_core.qsf')
foreach($macro in @('INCLUDE_CARTRIDGE','CHISBURNER_GBA_DUMP53','INCLUDE_CLK90','CHISBURNER_STORAGE_STREAM','CHISBURNER_STORAGE_AUDIT')){
 if($qsf -notmatch "(?m)^set_global_assignment -name VERILOG_MACRO $macro\s*$"){throw "Missing macro $macro"}
}
$policy=Get-Content -Raw (Join-Path $root 'core/cartridge_hal/dump53_policy.vh')
$values=@{READS_ENABLED='1';ADDRESS_SETUP='12';SELECT_SETUP='6';TURNAROUND='3';DATA_WAIT='18';RELEASE='6';MAX_READ_COMMANDS='272045';REQUIRED_READ_BYTES='32';READ_ADDRESS_MASK="32'h0000001f";READ_WINDOW_BYTES="32'h0084d500";READ_WINDOW_BASE="32'h00000000";EXACT_READ_WINDOW='0';FIXED_ADDRESS_SET='0'}
foreach($k in $values.Keys){if($policy -notmatch ('CHISBURNER_'+$k+'\s*=\s*'+[regex]::Escape($values[$k])+';')){throw "RTL policy mismatch $k"}}
$top=Get-Content -Raw (Join-Path $fpga 'core_top_dump53.v');if(!$top.Contains($policy)){throw 'Built top does not contain policy'}
foreach($stage in @('map','fit','asm','mif_update')){if((Get-Content -Raw (Join-Path $fpga "output_files/ap_core.$stage.rpt")) -notmatch 'was successful\. 0 errors'){throw "FPGA stage failed $stage"}}
$mif=Get-Content -Raw (Join-Path $fpga 'output_files/ap_core.mif_update.rpt')
if($mif -notmatch 'Processed Memory Initialization File .*firmware\.mif' -or $mif -notmatch 'Processed Memory Initialization File .*build_id\.mif' -or $mif -match 'Critical Warning|can.t find|setting all initial values to 0'){throw 'Boot MIF not verified'}
if((Sha (Join-Path $fpga 'firmware.mif')) -ne 'aade051a2a517f71d55f1610a3b4e6379e73896c68c322a96556f3982befbba2'){throw 'Boot MIF changed'}
$sta=Get-Content -Raw (Join-Path $fpga 'output_files/ap_core.sta.rpt');$slack=@{}
foreach($k in @('setup','hold')){$m=[regex]::Matches($sta,"Worst-case $k slack is\s+(-?\d+(?:\.\d+)?)");if(!$m.Count){throw 'Missing timing'};$slack[$k]=($m|ForEach-Object{[double]$_.Groups[1].Value}|Measure-Object -Minimum).Minimum;if($slack[$k] -le 0){throw "Timing fail $k"}}
if((Sha (Join-Path $root '.integration/openfpgaos/src/firmware/os/bld/pocket-output/os.bin')) -ne 'e87121ba586e2a43eb99aa522e055e678a964199e1802968c11ef06d76dfebaa'){throw 'Symbol provenance OS mismatch'}
$nm=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && riscv64-unknown-elf-nm -n .integration/openfpgaos/src/firmware/os/bld/pocket-output/firmware.elf")
if($LASTEXITCODE -ne 0){throw 'Symbol listing failed'}
foreach($s in @('1032e200 T svc_cart_get_caps.lto_priv.0','1032e340 T svc_cart_read.lto_priv.0','1032e3c0 T svc_cart_read_space.lto_priv.0','1032f180 T svc_output_open.lto_priv.0','1032f400 T svc_output_write.lto_priv.0','10328100 T of_file_async_busy','103297c0 T of_file_size','1038e530 b output_is_open','1038e534 b output_open_size','1038e538 b output_open_slot')){if($nm -cnotcontains $s){throw "Symbol mismatch $s"}}
$old=[IO.File]::ReadAllBytes((Join-Path $base $osRel));$null=Assert-ChisBurnerOsIntegrity -Bytes $old;$os=[byte[]]$old.Clone()
$ranges=@(@('.caps54',0xE200,0x100),@('.entry54',0xE340,0x40),@('.space54',0xE3C0,0x40),@('.patch54',0xF180,0x280))
foreach($r in $ranges){
 Wsl "cd '$wroot' && riscv64-unknown-elf-objcopy -O binary -j $($r[0]) build/dump54-app/services.elf build/dump54-app/$($r[0]).bin"
 $b=[IO.File]::ReadAllBytes((Join-Path $build "$($r[0]).bin"));if(!$b.Length -or $b.Length -gt $r[2]){throw 'OS service size outside region'}
 for($i=$r[1];$i -lt $r[1]+$r[2];$i+=4){[Array]::Copy([byte[]](0x13,0,0,0),0,$os,$i,4)}
 [Array]::Copy($b,0,$os,$r[1],$b.Length)
}
$os=Set-ChisBurnerOsCrc -Bytes $os;$changed=0
for($i=0;$i -lt $old.Length-4;++$i){if($os[$i] -ne $old[$i]){
 ++$changed;$allowed=$false;foreach($r in $ranges){if($i -ge $r[1] -and $i -lt $r[1]+$r[2]){$allowed=$true}};if(!$allowed){throw 'Unexpected OS byte modification'}
}}
if($changed -ne 2){throw "Expected only filename + version instruction changes, got $changed"}
if($os.Length -ne 137480){throw 'OS layout changed'};$crc=Assert-ChisBurnerOsIntegrity -Bytes $os
$runtime=@('app/gba_dump54/main.c','app/gba_dump54/dump.h','app/gba_dump53/md5.h','app/gba_dump54/read_block.h','app/gba_dump54/reference.h')
foreach($p in $runtime){$s=Get-Content -Raw (Join-Path $root $p);if($s -match '0x40000|\bfopen\s*\(|\bof_output_flush\s*\(|OF_CART_SPACE_SAVE|OF_CART_SPACE_RTC'){throw "Forbidden app operation $p"}}
$elf=[IO.File]::ReadAllBytes((Join-Path $build 'app.elf'))
if(![Text.Encoding]::ASCII.GetString($elf).Contains('GBA DUMP smoke.54') -or $elf.Length -gt 262144 -or $elf[0] -ne 0x7f -or $elf[4] -ne 1 -or $elf[5] -ne 1 -or [BitConverter]::ToUInt16($elf,18) -ne 243){throw 'Wrong application ELF'}
$demo=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && build/dump54-app/pc-test")
if($LASTEXITCODE -ne 0){throw 'UI demo failed'}
$demo=$demo|ForEach-Object{[regex]::Replace([string]$_,'\x1b\[[0-9;]*[A-Za-z]','')}
$pages=0;$rows=0;$maxRows=0;$width=0
foreach($line in $demo){if($line -eq ' GBA DUMP smoke.54'){$maxRows=[Math]::Max($maxRows,$rows);$rows=0;++$pages};++$rows;$width=[Math]::Max($width,$line.Length);if($line.Length -gt 40){throw "UI too wide: $line"}}
$maxRows=[Math]::Max($maxRows,$rows);if($maxRows -gt 18 -or $pages -ne 24){throw "UI shape $pages/$maxRows"}
# New artifact only. No existing package, source ROM/SAV, or physical SD writes.
$sd=Join-Path $bundle 'SD_ROOT';$basePrefix=[IO.Path]::GetFullPath($base).TrimEnd('\')+'\'
New-Item -ItemType Directory -Path $bundle | Out-Null
foreach($f in Get-ChildItem -LiteralPath $base -Recurse -File){$rel=$f.FullName.Substring($basePrefix.Length).Replace('\','/');if($rel -in $omitted){continue};$dest=Join-Path $sd $rel;New-Item -ItemType Directory -Force (Split-Path $dest -Parent)|Out-Null;Copy-Item -LiteralPath $f.FullName -Destination $dest}
[IO.File]::WriteAllBytes((Join-Path $sd $osRel),$os)
Copy-Item -LiteralPath (Join-Path $build 'app.elf') -Destination (Join-Path $sd $appRel)
[IO.File]::WriteAllText((Join-Path $sd $iniRel),"[os]`nELF=chisburner54.elf`nARGS=`n",$utf8)
$core=Get-Content -Raw (Join-Path $sd $coreRel)|ConvertFrom-Json
$core.core.framework.hardware.cartridge_adapter=16777216
$core.core.metadata.version='0.1.0-smoke.54-gba-dump';$core.core.metadata.date_release='2026-09-04'
[IO.File]::WriteAllText((Join-Path $sd $coreRel),($core|ConvertTo-Json -Depth 25),$utf8)
$data=Get-Content -Raw (Join-Path $sd $dataRel)|ConvertFrom-Json
foreach($slot in $data.data.data_slots){switch([int]$slot.id){2{$slot.filename='chisburner54.ini'}3{$slot.filename='chisburner54.elf'}4{$slot.name='GBA Dump 54';$slot.filename='dump54.gba';$slot.extensions=@('gba');$slot.size_exact=8705280;if([int]$slot.parameters -ne 2 -or !$slot.deferload -or $slot.nonvolatile -or $slot.address){throw 'Wrong slot4 policy'}}}}
[IO.File]::WriteAllText((Join-Path $sd $dataRel),($data|ConvertTo-Json -Depth 25),$utf8)
New-Item -ItemType Directory -Force (Split-Path (Join-Path $sd $fixtureRel) -Parent)|Out-Null
[IO.File]::WriteAllBytes((Join-Path $sd $fixtureRel),(New-Object byte[] 8705280))
if((Get-FileHash -LiteralPath (Join-Path $sd $fixtureRel) -Algorithm MD5).Hash -ne '92391F2C50FAA2E6A0C99160BB8BA097'){throw 'Seed not exact zero fixture'}
# Byte-identical real-tested FPGA; no new synthesis or MIF changes in smoke54.
if((Sha (Join-Path $base $rbfRel)) -ne (Sha (Join-Path $sd $rbfRel))){throw 'RBF changed'}
foreach($f in Get-ChildItem -LiteralPath $base -Recurse -File){$rel=$f.FullName.Substring($basePrefix.Length).Replace('\','/');if($rel -in ($omitted+@($coreRel,$dataRel,$osRel,$rbfRel))){continue};if((Sha $f.FullName) -ne (Sha (Join-Path $sd $rel))){throw "Unexpected changed payload $rel"}}
foreach($id in 1,2,3,4){$slot=$data.data.data_slots|Where-Object id -eq $id;$dir=if($id -eq 4){'Assets/chisburner/ChisBurner.ChisBurner'}else{'Assets/chisburner/common'};$file=Join-Path (Join-Path $sd $dir) $slot.filename;if(!(Test-Path -LiteralPath $file)){throw 'Missing default slot file'};if($id -eq 4 -and (Get-Item -LiteralPath $file).Length -ne $slot.size_exact){throw 'Slot size mismatch'}}
if(Test-Path (Join-Path $sd 'Saves')){throw 'No user saves allowed in bundle'}
Copy-Item -LiteralPath (Join-Path $root 'app/gba_dump54/README_TEST.txt') -Destination $bundle
Copy-Item -LiteralPath (Join-Path $root 'tests/Compare-ROM54.ps1') -Destination $bundle
Copy-Item -LiteralPath (Join-Path $fpga 'SOURCE_SHA256.txt') -Destination (Join-Path $bundle 'FPGA_SOURCE_SHA256.txt')
[IO.File]::WriteAllLines((Join-Path $bundle 'UI_FIXTURES_SYNTHETIC.txt'),[string[]]$demo,$utf8)
$sources=@($runtime+$reuse+@('app/gba_dump54/test_dump54.c','app/gba_dump54/Makefile','app/gba_dump54/README_TEST.txt','tests/build-dump54.ps1','tests/build-dump53-fpga.ps1','tests/os-image-integrity.ps1','tests/Compare-ROM54.ps1','tests/test-dump54-instructions.cpp','tests/test-dump54-services.c','tests/test-dump53-rtl.cpp','tests/patches/dump54.ld','tests/patches/dump54_entry.S','.integration/openfpgaos/src/firmware/os/targets/pocket/dump54_services.c','.integration/openfpgaos/src/firmware/api/of_cart.h','.integration/openfpgaos/src/firmware/api/of_file.h','.integration/openfpgaos/src/firmware/api/of_output.h'))|Select-Object -Unique
[IO.File]::WriteAllLines((Join-Path $bundle 'SOFTWARE_SOURCE_SHA256.txt'),[string[]]@($sources|ForEach-Object{"$(Sha (Join-Path $root $_))  $_"}),$utf8)
$proof=@"
smoke=54-gba-dump-timing32k
hardware_result=PENDING;32KiB TF flow + timing NOT tested on Pocket by build host
baseline_real_user_report=smoke53 full ROM MD5 + cold boot ROM VERIFIED;431 seconds
performance=TF per-pass call count 2126 to 266;cart calls remain 272045;speedup MUST be measured
milliseconds=cart,TFwrite,blockreadback,hash+compare,safe,finalTFMD5,other/UI,total;boot separate
source_ROM_sha256=$romHash
source_ROM_size=8705280
source_ROM_md5=07e851b1dc49009ebd00982514ebfe92
source_SAV_sha256=$savHash
source_SAV_size=32768;not accessed by app;not included in package
FPGA_setup_slack_ns=$($slack['setup'])
FPGA_hold_slack_ns=$($slack['hold'])
FPGA_compile_MIF_update_asm=accepted smoke53 artifacts; NOT rebuilt for smoke54
FPGA_272045_reads_ROM_only_limit_and_pins=PASS Verilator
FPGA_boot_MIF_and_storage_bridge_sources=unchanged from accepted smoke53
SDK_app_build=PASS
app_MD5_vectors_exact_source_full_flow_fault_DMA_disjoint_timing_rollover_tests=PASS ASan UBSan
actual_linked_OS_caps_read_read_space_bind_ABI_allowlist=PASS ASan UBSan runner
native_OS_service_fault_tests=PASS ASan UBSan
accepted_input_gate_regression=PASS
OS_base=smoke53;length137480;only output filename and version magic changed; read/write logic unchanged
OS_allowed_regions=E200..E2FF,E340..E37F,E3C0..E3FF,F180..F3FF,final4 CRC bytes; exact changed bytes excl CRC=2
OS_changed_bytes_excluding_CRC=$changed
OS_internal_CRC=$('{0:x8}' -f $crc.ComputedCrc)
OS_length=$($os.Length)
OS_sha256=$(Sha (Join-Path $sd $osRel))
RBF_sha256=$(Sha (Join-Path $sd $rbfRel))
app_sha256=$(Sha (Join-Path $sd $appRel))
entry=openFPGA / ChisBurner / Play Cartridge;fixed defaults 1,2,3,4;no instance needed
static_output=dump54.gba;8705280 zero bytes;265x32768+21760
runtime=boot full TF scan;release/LR/A/START one-shot;5 cart identity samples;static bind;cart32x272040;TF32KiBx266+immediate compare;full cartMD5 and independent full TFMD5
cart_writes_save_erase_voltage_switches=structurally unavailable
FLUSH=never called;previous -12 NOT fixed or converted to success
TF_host_OPEN_resize=not used;static binding only
unknown_or_verified_boot_file=READ_ONLY;no overwrite or retry
ui_synthetic_pages=$pages;max_columns=$width;max_rows=$maxRows
simulated_Pocket_host_firmware_or_electrical_behaviour=NOT_CLAIMED
persistence=requires real normal Quit then full power off/on;not abrupt-power-loss guarantee
"@
[IO.File]::WriteAllText((Join-Path $bundle 'BUILD_PROVENANCE.txt'),$proof,$utf8)
if((Sha $rom) -ne $romHash -or (Sha $sav) -ne $savHash){throw 'Reference modified during build'}
$prefix=[IO.Path]::GetFullPath($sd).TrimEnd('\')+'\';$hashes=@{}
$sums=Get-ChildItem -LiteralPath $sd -Recurse -File|Sort-Object FullName|ForEach-Object{$rel=$_.FullName.Substring($prefix.Length).Replace('\','/');$h=Sha $_.FullName;$hashes["SD_ROOT/$rel"]=$h;"$h  $rel"}
if($hashes.Count -ne 15){throw "Unexpected payload count $($hashes.Count)"}
[IO.File]::WriteAllLines((Join-Path $bundle 'SHA256SUMS.txt'),[string[]]$sums,$utf8)
Compress-Archive -Path (Join-Path $bundle '*') -DestinationPath $zip -CompressionLevel Optimal
$null=Assert-ChisBurnerZipOsIntegrity -ZipPath $zip
$archive=[IO.Compression.ZipFile]::OpenRead($zip)
try{foreach($name in $hashes.Keys){$e=$archive.GetEntry($name);if(!$e){throw "Missing ZIP payload $name"};$s=$e.Open();$h=[Security.Cryptography.SHA256]::Create();try{$actual=[BitConverter]::ToString($h.ComputeHash($s)).Replace('-','').ToLowerInvariant();if($actual -ne $hashes[$name]){throw "ZIP mismatch $name"}}finally{$s.Dispose();$h.Dispose()}}}finally{$archive.Dispose()}
"PASS: $zip"
"ZIP SHA256: $(Sha $zip)"
"OS CRC + four-region allowlist + all15 ZIP payload hashes verified; UI $pages pages $width columns/$maxRows rows"
