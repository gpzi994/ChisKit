$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
Set-Location $root
$baseline='build/smoke/ChisBurner-smoke.126-sdram-noflush-r1'
$bundle='build/smoke/ChisBurner-smoke.128-sdram-file-save-r8'
$fpga='build/psram128-r8/quartus'
$os='.integration/openfpgaos/src/firmware/os/bld/psram128'
function Sha($p){(Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash.ToLowerInvariant()}
if((Test-Path $bundle)-or(Test-Path "$bundle.zip")){throw 'Preserve existing package'}
foreach($p in @("$fpga/output_files/ap_core.rbf","$fpga/firmware.mif","$os/firmware.mif","$os/os.bin","$os/firmware.elf")){
 if(!(Test-Path -LiteralPath $p)){throw "Missing build input: $p"}
}
if((Sha "$fpga/firmware.mif") -cne (Sha "$os/firmware.mif")){throw 'BRAM firmware mismatch'}
if((Get-Content "$fpga/update-mif.log" -Raw) -notmatch 'Processed Memory Initialization File'){throw 'Missing r8 MIF update'}
if((Get-Content "$fpga/assemble.log" -Raw) -notmatch 'Assembler was successful. 0 errors'){throw 'r8 assembly failed'}
if((Get-Content "$fpga/full-compile.log" -Raw) -notmatch 'Full Compilation was successful. 0 errors'){throw 'Full FPGA compile did not succeed'}
if((Get-Content 'build/psram128-r7-seed2/refit.log' -Raw) -notmatch 'Quartus Prime Shell was successful. 0 errors'){throw 'Seed 2 refit did not succeed'}
& "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot "$root/$fpga"
. "$PSScriptRoot/os-image-integrity.ps1"
$bytes=[IO.File]::ReadAllBytes("$root/$os/os.bin")
$text=[Text.Encoding]::ASCII.GetString($bytes)
foreach($needle in @('ChisBurner smoke.128-r8','Output compare 1024 PASS','File/save paths: SDRAM only','PSRAM0 old-use count 0','sdram128-output.bin')){
 if(!$text.Contains($needle)){throw "Missing diagnostic marker: $needle"}
}
$null=Assert-ChisBurnerOsIntegrity -Bytes $bytes
$symbols=Get-Content build/psram128/symbols.txt -Raw
if($symbols -match '\bboot_cram0_mode\b|\bboot_cram_warmup\b'){throw 'CRAM0 boot routine survived PSRAM128 compile'}
$utf8=[Text.UTF8Encoding]::new($false)
function Json($p,$o){[IO.File]::WriteAllText("$root/$p",($o|ConvertTo-Json -Depth 20),$utf8)}
if(-not('ChisBurner.Ps128.Reverse'-as[type])){Add-Type -TypeDefinition @'
namespace ChisBurner.Ps128 { public static class Reverse { public static byte[] Apply(byte[] a) { var t=new byte[256]; for(int i=0;i<256;i++){int x=i,y=0;for(int b=0;b<8;b++){y=(y<<1)|(x&1);x>>=1;}t[i]=(byte)y;}var r=new byte[a.Length];for(int i=0;i<a.Length;i++)r[i]=t[a[i]];return r; } } }
'@}
$core="$bundle/SD_ROOT/Cores/ChisBurner.PSRAM128"
$assets="$bundle/SD_ROOT/Assets/chisburnerps128/common"
foreach($dir in @($core,$assets,"$bundle/SD_ROOT/Platforms")){New-Item -ItemType Directory -Path $dir -Force|Out-Null}
$oldcore="$baseline/SD_ROOT/Cores/ChisBurner.SDRAM123"
foreach($name in @('audio.json','video.json','input.json','interact.json','variants.json','loader.bin')){Copy-Item "$oldcore/$name" "$core/$name"}
$meta=Get-Content "$oldcore/core.json" -Raw|ConvertFrom-Json
$meta.core.metadata.platform_ids=@('chisburnerps128')
$meta.core.metadata.shortname='PSRAM128'
$meta.core.metadata.description='ChisBurner SDRAM-only file, config and save transport diagnostic.'
$meta.core.metadata.version='0.1.0-smoke.128-sdram-file-save-r8'
$meta.core.metadata.date_release='2026-09-13'
Json "$core/core.json" $meta
$data=[ordered]@{data=[ordered]@{magic='APF_VER_1';data_slots=@(
 [ordered]@{id=0;name='Diagnostic';required=$false;parameters=275;extensions=@('json')},
 [ordered]@{id=1;name='OS Binary';required=$false;parameters=0;extensions=@('bin');deferload=$true;filename='os.bin'},
 [ordered]@{id=4;name='SDRAM input';required=$false;parameters=0;extensions=@('bin');deferload=$true;filename='sdram128-input.bin';size_exact=4096},
 [ordered]@{id=5;name='Dynamic output';required=$false;parameters=0;extensions=@('bin');deferload=$true},
 [ordered]@{id=8;name='SDRAM config';required=$false;parameters='0x84';extensions=@('cfg','bin');deferload=$true;filename='sdram128-config.bin';size_maximum='0x40000'},
 [ordered]@{id=10;name='SDRAM save';required=$false;parameters='0x84';extensions=@('sav','bin');deferload=$true;filename='sdram128-save.bin';size_maximum='0x40000'}
)}}
Json "$core/data.json" $data
$platform=Get-Content "$baseline/SD_ROOT/Platforms/chisburnerdiag.json" -Raw|ConvertFrom-Json
$platform.platform.name='ChisBurner PSRAM128'
Json "$bundle/SD_ROOT/Platforms/chisburnerps128.json" $platform
[IO.File]::WriteAllBytes("$root/$core/chis.rbf_r",[ChisBurner.Ps128.Reverse]::Apply([IO.File]::ReadAllBytes("$root/$fpga/output_files/ap_core.rbf")))
Copy-Item "$os/os.bin" "$assets/os.bin"
[byte[]]$inputFixture=New-Object byte[] 4096
[byte[]]$config=New-Object byte[] 4096
[byte[]]$save=New-Object byte[] 4096
for($i=0;$i-lt 4096;$i++){
 $inputFixture[$i]=[byte](($i*37+0x5a)-band 0xff)
 $config[$i]=[byte](($i*37+0x33)-band 0xff)
 $save[$i]=[byte](($i*37+0x11)-band 0xff)
}
[IO.File]::WriteAllBytes("$root/$assets/sdram128-input.bin",$inputFixture)
[IO.File]::WriteAllBytes("$root/$assets/sdram128-config.bin",$config)
[IO.File]::WriteAllBytes("$root/$assets/sdram128-save.bin",$save)
[IO.File]::WriteAllText("$root/$core/info.txt",'smoke.128 SDRAM-only file/config/save diagnostic. PSRAM0 is untouched. No cartridge operation. Read TEST_CN.md.',$utf8)
Copy-Item docs/SMOKE128_TEST_CN.md "$bundle/TEST_CN.md"
Copy-Item tests/verify-psram128-output.ps1 "$bundle/VERIFY_OUTPUT.ps1"
$receipt=@()
foreach($p in @("$os/firmware.elf","$os/firmware.mif","$os/os.bin","$fpga/output_files/ap_core.rbf","$fpga/ap_core.qsf","$fpga/full-compile.log","$fpga/assemble.log",'build/psram128-r7-seed2/refit.log','build/psram128/sections.txt','build/psram128/symbols.txt','build/psram128/disassembly.txt')){$receipt+="$(Sha $p)  $p"}
[IO.File]::WriteAllLines("$root/$bundle/BUILD_INPUTS_SHA256.txt",[string[]]$receipt,$utf8)
$sources=@('tests/build-psram128-os.sh','tests/finish-psram128.ps1','tests/build-psram128-package.ps1','tests/check-psram128-getfile.ps1','tests/check-psram128-flush-admission.ps1','tests/rtl/tb_sdram120_apf.cpp','tests/configure-psram128-seed.tcl','tests/refit-psram128-seed.tcl','docs/SMOKE128_TEST_CN.md','.integration/openfpgaos/src/fpga/targets/pocket/core_top.v','.integration/openfpgaos/src/fpga/targets/pocket/core_bridge_cmd.v','.integration/openfpgaos/src/firmware/os/targets/pocket/boot/boot.c','.integration/openfpgaos/src/firmware/os/targets/pocket/file.c','.integration/openfpgaos/src/firmware/os/targets/pocket/save.c','.integration/openfpgaos/src/firmware/os/hal/file.h','.integration/openfpgaos/src/firmware/os/kernel/main.c','.integration/openfpgaos/src/firmware/os/targets/pocket/target_platform.h','core/sdram/file122.h','core/sdram/driver117.h','core/sdram/control116.h')
[IO.File]::WriteAllLines("$root/$bundle/SOURCES_SHA256.txt",[string[]]@($sources|ForEach-Object{"$(Sha $_)  $_"}),$utf8)
$prefix=[IO.Path]::GetFullPath("$bundle/SD_ROOT").TrimEnd('\')+'\'
$sums=@(Get-ChildItem "$bundle/SD_ROOT" -Recurse -File|Sort-Object FullName|ForEach-Object{"$(Sha $_.FullName)  $($_.FullName.Substring($prefix.Length).Replace('\','/'))"})
[IO.File]::WriteAllLines("$root/$bundle/SHA256SUMS.txt",[string[]]$sums,$utf8)
Compress-Archive -Path "$bundle/*" -DestinationPath "$bundle.zip"
Add-Type -AssemblyName System.IO.Compression.FileSystem
$zip=[IO.Compression.ZipFile]::OpenRead("$root/$bundle.zip")
try {
 foreach($line in $sums){
  $hash,$path=$line -split '  ',2
  $entry=$zip.Entries|Where-Object {$_.FullName.Replace('\','/') -ceq "SD_ROOT/$path"};if(!$entry){throw "Missing ZIP entry $path"}
  $stream=$entry.Open();$sha=[Security.Cryptography.SHA256]::Create()
  try{$actual=([BitConverter]::ToString($sha.ComputeHash($stream))).Replace('-','').ToLowerInvariant()}finally{$stream.Dispose();$sha.Dispose()}
  if($actual -cne $hash){throw "ZIP hash mismatch $path"}
 }
}finally{$zip.Dispose()}
Get-FileHash "$bundle.zip" -Algorithm SHA256
