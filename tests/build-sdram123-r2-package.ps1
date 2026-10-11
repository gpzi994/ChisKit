$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
Set-Location $root
$baseline='build/smoke/ChisBurner-smoke.111-nor-test-r1'
$bundle='build/smoke/ChisBurner-smoke.123-sdram-roundtrip-r2'
$fpga='build/sdram123-r2/quartus'
$os='.integration/openfpgaos/src/firmware/os/bld/sdram123-r2-boot'
function Sha($p){(Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash.ToLowerInvariant()}
if((Test-Path $bundle)-or(Test-Path "$bundle.zip")){throw 'Preserve existing package'}
if((Sha "$baseline.zip") -ne 'f6a87f892c55f6dfeffc2f56cdbf7367acdfc146f02268790dc34045ec38cde0'){throw 'Stable package changed'}
foreach($line in Get-Content "$baseline/SHA256SUMS.txt") {
 if($line -notmatch '^([0-9a-f]{64})  (.+)$'){throw 'Invalid baseline receipt'}
 $hash=$Matches[1];$path=$Matches[2]
 if((Sha "$baseline/SD_ROOT/$path") -cne $hash){throw "Baseline input changed: $path"}
}
& "$PSScriptRoot/check-sdram121-reports.ps1" -reportRoot "$root/$fpga"
if((Sha "$fpga/firmware.mif") -cne (Sha "$os/firmware.mif")){throw 'BRAM firmware mismatch'}
if((Get-Content "$fpga/update-mif.log" -Raw) -notmatch 'Processed Memory Initialization File .*/sdram123-r2/quartus/firmware.mif'){throw 'Missing MIF update evidence'}
if((Get-Content "$fpga/assemble.log" -Raw) -notmatch 'Assembler was successful. 0 errors'){throw 'Assembler did not succeed'}
$fixture='build/sdram123-r2/fixture123.bin'
& "$PSScriptRoot/verify-sdram123-output.ps1" -Path $fixture
. "$PSScriptRoot/os-image-integrity.ps1"
$bytes=[IO.File]::ReadAllBytes("$root/$os/os.bin")
$imageText=[Text.Encoding]::ASCII.GetString($bytes)
if(-not $imageText.Contains('/Assets/chisburnerdiag/common/sdram122-roundtrip.bin')){throw 'Missing corrected APF output path'}
if(-not $imageText.Contains('ChisBurner smoke.123-r2')){throw 'Wrong diagnostic OS revision'}
$null=Assert-ChisBurnerOsIntegrity -Bytes $bytes
$old=[IO.File]::ReadAllBytes("$root/$baseline/SD_ROOT/Assets/chisburner/common/os.bin")
foreach($image in @($bytes,$old)) {
 if([Text.Encoding]::ASCII.GetString($image,$image.Length-32,4) -cne 'OABI'){throw 'Missing OABI'}
 if([Text.Encoding]::ASCII.GetString($image,$image.Length-24,4) -cne 'OSE2'){throw 'Missing OSE2'}
}
if([BitConverter]::ToUInt32($bytes,$bytes.Length-28) -ne [BitConverter]::ToUInt32($old,$old.Length-28)){throw 'Boot ABI mismatch'}
$symbols=@{}
foreach($line in Get-Content build/sdram123-r2/symbols.txt){
 if($line -match '^([0-9a-f]+) \w (\S+)$'){$symbols[$Matches[2]]=[Convert]::ToUInt32($Matches[1],16)}
}
if([BitConverter]::ToUInt32($bytes,$bytes.Length-20) -ne $symbols.os_main){throw 'Entry metadata mismatch'}
if([BitConverter]::ToUInt32($bytes,$bytes.Length-16) -ne $symbols.__os_bss_start){throw 'BSS metadata mismatch'}
if([BitConverter]::ToUInt32($bytes,$bytes.Length-12) -ne $symbols.__os_bss_end){throw 'BSS end mismatch'}
if(0x10320000L+$bytes.Length -gt $symbols.sd122_buffers+64L){throw 'OS trailer overlaps DMA payload'}
if($symbols.sd122_buffers+65600L -gt $symbols.__os_bss_start){throw 'DMA overlaps BSS'}
if($symbols.__os_bss_end -ge 0x103e0000L){throw 'OS diagnostic exceeds reserved OSDATA'}
if((Get-Content build/sdram123-r2/sections.txt -Raw) -notmatch '\.sd122_nocache\s+NOBITS'){throw 'DMA buffer not NOLOAD'}
if(-not('ChisBurner.Sd123.Reverse'-as[type])){Add-Type -TypeDefinition @'
namespace ChisBurner.Sd123 { public static class Reverse { public static byte[] Apply(byte[] a) { var t=new byte[256]; for(int i=0;i<256;i++){int x=i,y=0;for(int b=0;b<8;b++){y=(y<<1)|(x&1);x>>=1;}t[i]=(byte)y;}var r=new byte[a.Length];for(int i=0;i<a.Length;i++)r[i]=t[a[i]];return r; } } }
'@}
$utf8=[Text.UTF8Encoding]::new($false)
function Json($p,$o){[IO.File]::WriteAllText("$root/$p",($o|ConvertTo-Json -Depth 20),$utf8)}
$core="$bundle/SD_ROOT/Cores/ChisBurner.SDRAM123"
$assets="$bundle/SD_ROOT/Assets/chisburnerdiag/common"
foreach($dir in @($core,$assets,"$bundle/SD_ROOT/Platforms")){New-Item -ItemType Directory -Path $dir -Force|Out-Null}
$oldcore="$baseline/SD_ROOT/Cores/ChisBurner.ChisBurner"
foreach($name in @('audio.json','video.json','input.json','interact.json','variants.json','loader.bin')){Copy-Item "$oldcore/$name" "$core/$name"}
$meta=Get-Content "$oldcore/core.json" -Raw|ConvertFrom-Json
$meta.core.metadata.platform_ids=@('chisburnerdiag')
$meta.core.metadata.shortname='SDRAM123'
$meta.core.metadata.description='ChisBurner TF-SDRAM roundtrip diagnostic. No cartridge required.'
$meta.core.metadata.version='0.1.0-smoke.123-r2-sdram-diag'
$meta.core.metadata.date_release='2026-09-09'
Json "$core/core.json" $meta
$data=Get-Content "$oldcore/data.json" -Raw|ConvertFrom-Json
$data.data.data_slots=@($data.data.data_slots|Where-Object {$_.id -in @(0,1,2,4,5)})
foreach($slot in $data.data.data_slots){
 if($slot.id -eq 0){$slot.required=$false;$slot.name='Diagnostic'}
 if($slot.id -eq 2){$slot.filename='diag123.ini'}
 if($slot.id -eq 4){$slot.name='Known pattern';$slot.parameters=0;$slot.extensions=@('bin');$slot|Add-Member filename 'fixture123.bin'}
 if($slot.id -eq 5){$slot.name='Diagnostic output';$slot.extensions=@('bin')}
}
Json "$core/data.json" $data
$platform=Get-Content "$baseline/SD_ROOT/Platforms/chisburner.json" -Raw|ConvertFrom-Json
$platform.platform.name='ChisBurner SDRAM123'
Json "$bundle/SD_ROOT/Platforms/chisburnerdiag.json" $platform
[IO.File]::WriteAllBytes("$root/$core/chis.rbf_r",[ChisBurner.Sd123.Reverse]::Apply([IO.File]::ReadAllBytes("$root/$fpga/output_files/ap_core.rbf")))
Copy-Item "$os/os.bin" "$assets/os.bin"
Copy-Item $fixture "$assets/fixture123.bin"
[IO.File]::WriteAllText("$root/$assets/diag123.ini","[os]`nCORE=os25`n",$utf8)
[IO.File]::WriteAllText("$root/$core/info.txt","smoke.123-r2 TF-SDRAM diagnostic. Remove cartridge while powered off. Automatic test; no ROM required. Read TEST_CN.md.",$utf8)
Copy-Item docs/SMOKE123_R2_TEST_CN.md "$bundle/TEST_CN.md"
Copy-Item tests/verify-sdram123-output.ps1 "$bundle/VERIFY_OUTPUT.ps1"
Copy-Item docs/SMOKE123_R2_TEST_CN.md "$bundle/DEVELOPMENT_NOTES.md"
$receipt=@()
foreach($p in @("$os/firmware.elf","$os/firmware.mif","$os/os.bin","$fpga/output_files/ap_core.rbf","$fpga/ap_core.qsf","$fpga/update-mif.log","$fpga/assemble.log",'build/sdram123-r2/sections.txt','build/sdram123-r2/symbols.txt')){$receipt+="$(Sha $p)  $p"}
[IO.File]::WriteAllLines("$root/$bundle/BUILD_INPUTS_SHA256.txt",[string[]]$receipt,$utf8)
$sources=@('core/sdram/file122.h','core/sdram/driver117.h','core/sdram/control116.h','core/sdram/fixture123.h','tests/build-sdram123-r2-package.ps1','tests/build-sdram123-r2-os.sh','tests/build-sdram123-r2-fpga.ps1','.integration/openfpgaos/src/firmware/os/targets/pocket/file_sdram122.inc','.integration/openfpgaos/src/firmware/os/kernel/main.c','.integration/openfpgaos/src/firmware/os/os.ld','.integration/openfpgaos/src/firmware/os/Makefile')
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
