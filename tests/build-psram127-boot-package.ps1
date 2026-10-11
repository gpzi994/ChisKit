$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
Set-Location $root
$baseline='build/smoke/ChisBurner-smoke.126-sdram-noflush-r1'
$bundle='build/smoke/ChisBurner-smoke.127-psram0-evacuated-boot-r2'
$fpga='build/psram127-boot-seed6/quartus'
$os='.integration/openfpgaos/src/firmware/os/bld/psram127-boot-r2'
function Sha($p){(Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash.ToLowerInvariant()}
if((Test-Path $bundle)-or(Test-Path "$bundle.zip")){throw 'Preserve existing package'}
foreach($p in @("$fpga/output_files/ap_core.rbf","$fpga/firmware.mif","$os/firmware.mif","$os/os.bin","$os/firmware.elf")){
 if(!(Test-Path -LiteralPath $p)){throw "Missing build input: $p"}
}
if((Sha "$fpga/firmware.mif") -cne (Sha "$os/firmware.mif")){throw 'BRAM firmware mismatch'}
if((Get-Content "$fpga/update-mif.log" -Raw) -notmatch 'Processed Memory Initialization File'){throw 'Missing MIF update evidence'}
if((Get-Content "$fpga/assemble.log" -Raw) -notmatch 'Assembler was successful. 0 errors'){throw 'Assembler did not succeed'}
& "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot "$root/$fpga"
. "$PSScriptRoot/os-image-integrity.ps1"
$bytes=[IO.File]::ReadAllBytes("$root/$os/os.bin")
$text=[Text.Encoding]::ASCII.GetString($bytes)
foreach($needle in @('ChisBurner smoke.127','PSRAM0 complete evacuation','OS: TF -> SDRAM direct','CRAM0 bridge words')){
 if(!$text.Contains($needle)){throw "Missing diagnostic marker: $needle"}
}
$null=Assert-ChisBurnerOsIntegrity -Bytes $bytes
$symbols=Get-Content build/psram127-boot/symbols.txt -Raw
if($symbols -match '\bboot_cram0_mode\b|\bboot_cram_warmup\b'){throw 'CRAM0 boot routine survived PSRAM127 compile'}
$utf8=[Text.UTF8Encoding]::new($false)
function Json($p,$o){[IO.File]::WriteAllText("$root/$p",($o|ConvertTo-Json -Depth 20),$utf8)}
if(-not('ChisBurner.Ps127.Reverse'-as[type])){Add-Type -TypeDefinition @'
namespace ChisBurner.Ps127 { public static class Reverse { public static byte[] Apply(byte[] a) { var t=new byte[256]; for(int i=0;i<256;i++){int x=i,y=0;for(int b=0;b<8;b++){y=(y<<1)|(x&1);x>>=1;}t[i]=(byte)y;}var r=new byte[a.Length];for(int i=0;i<a.Length;i++)r[i]=t[a[i]];return r; } } }
'@}
$core="$bundle/SD_ROOT/Cores/ChisBurner.PSRAM127"
$assets="$bundle/SD_ROOT/Assets/chisburnerps127/common"
foreach($dir in @($core,$assets,"$bundle/SD_ROOT/Platforms")){New-Item -ItemType Directory -Path $dir -Force|Out-Null}
$oldcore="$baseline/SD_ROOT/Cores/ChisBurner.SDRAM123"
foreach($name in @('audio.json','video.json','input.json','interact.json','variants.json','loader.bin')){Copy-Item "$oldcore/$name" "$core/$name"}
$meta=Get-Content "$oldcore/core.json" -Raw|ConvertFrom-Json
$meta.core.metadata.platform_ids=@('chisburnerps127')
$meta.core.metadata.shortname='PSRAM127'
$meta.core.metadata.description='ChisBurner direct TF-to-SDRAM boot; zero PSRAM0 access diagnostic.'
$meta.core.metadata.version='0.1.0-smoke.127-psram0-evacuated-boot-r2'
$meta.core.metadata.date_release='2026-09-13'
Json "$core/core.json" $meta
$data=[ordered]@{data=[ordered]@{magic='APF_VER_1';data_slots=@(
 [ordered]@{id=0;name='Diagnostic';required=$false;parameters=275;extensions=@('json')},
 [ordered]@{id=1;name='OS Binary';required=$false;parameters=0;extensions=@('bin');deferload=$true;filename='os.bin'}
)}}
Json "$core/data.json" $data
$platform=Get-Content "$baseline/SD_ROOT/Platforms/chisburnerdiag.json" -Raw|ConvertFrom-Json
$platform.platform.name='ChisBurner PSRAM127'
Json "$bundle/SD_ROOT/Platforms/chisburnerps127.json" $platform
[IO.File]::WriteAllBytes("$root/$core/chis.rbf_r",[ChisBurner.Ps127.Reverse]::Apply([IO.File]::ReadAllBytes("$root/$fpga/output_files/ap_core.rbf")))
Copy-Item "$os/os.bin" "$assets/os.bin"
[IO.File]::WriteAllText("$root/$core/info.txt","smoke.127 direct TF-to-SDRAM boot diagnostic. PSRAM0 is untouched. Remove cartridge while powered off. Read TEST_CN.md.",$utf8)
Copy-Item docs/SMOKE127_TEST_CN.md "$bundle/TEST_CN.md"
$receipt=@()
foreach($p in @("$os/firmware.elf","$os/firmware.mif","$os/os.bin","$fpga/output_files/ap_core.rbf","$fpga/ap_core.qsf","$fpga/update-mif.log","$fpga/assemble.log",'build/psram127-boot/sections.txt','build/psram127-boot/symbols.txt')){$receipt+="$(Sha $p)  $p"}
[IO.File]::WriteAllLines("$root/$bundle/BUILD_INPUTS_SHA256.txt",[string[]]$receipt,$utf8)
$sources=@('tests/build-psram127-boot-os.sh','tests/finish-psram127-boot.ps1','tests/build-psram127-boot-package.ps1','docs/SMOKE127_TEST_CN.md','.integration/openfpgaos/src/firmware/os/targets/pocket/boot/boot.c','.integration/openfpgaos/src/firmware/os/hal/memtest.c','.integration/openfpgaos/src/firmware/os/kernel/main.c','.integration/openfpgaos/src/firmware/os/targets/pocket/target_platform.h','.integration/openfpgaos/src/firmware/os/Makefile','.integration/openfpgaos/src/firmware/os/.contract.md5','core/rtl/chisburner_sdram_pocket120.v','core/rtl/chisburner_sdram_interlock119.v')
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
  $entry=$zip.GetEntry("SD_ROOT/$path");if(!$entry){throw "Missing ZIP entry $path"}
  $stream=$entry.Open();$sha=[Security.Cryptography.SHA256]::Create()
  try{$actual=([BitConverter]::ToString($sha.ComputeHash($stream))).Replace('-','').ToLowerInvariant()}finally{$stream.Dispose();$sha.Dispose()}
  if($actual -cne $hash){throw "ZIP hash mismatch $path"}
 }
}finally{$zip.Dispose()}
Get-FileHash "$bundle.zip" -Algorithm SHA256
