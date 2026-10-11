$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
Set-Location $root
$baseline='build/smoke/ChisBurner-smoke.130-serial-psram-nor-r1'
$bundle='build/smoke/ChisBurner-smoke.131-hw-psram-nor-r1'
$fpga='build/stream131-r1-seed6/quartus'
$os='.integration/openfpgaos/src/firmware/os/bld/stream131-release'
$app='build/ui131-release-app/app.elf'
function Sha($p){(Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash.ToLowerInvariant()}
if((Test-Path $bundle)-or(Test-Path "$bundle.zip")){throw 'Preserve existing Stream131 package'}
foreach($p in @($baseline,"$fpga/output_files/ap_core.rbf","$fpga/firmware.mif","$fpga/full-compile.log","$os/firmware.mif","$os/os.bin","$os/firmware.elf",$app)){
 if(!(Test-Path -LiteralPath $p)){throw "Missing build input: $p"}
}
if((Sha "$fpga/firmware.mif") -cne (Sha "$os/firmware.mif")){throw 'BRAM firmware mismatch'}
if((Get-Content "$fpga/full-compile.log" -Raw) -notmatch 'Full Compilation was successful. 0 errors'){throw 'Full FPGA compile did not succeed'}
& "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot "$root/$fpga"
. "$PSScriptRoot/os-image-integrity.ps1"
$null=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes("$root/$os/os.bin"))
$symbols=Get-Content build/stream131-release/symbols.txt -Raw
foreach($name in @('svc_psram_cache_load','svc_psram_cache_read','nor131_program_next','nor131_program_step','nor131_compare_start','nor131_compare_result')){
 if($symbols -notmatch "(?m)^[0-9a-f]+ [Tt] $([regex]::Escape($name))"){throw "Missing OS service symbol: $name"}
}
$appText=[Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes("$root/$app"))
foreach($marker in @('smoke.131','TF -> PSRAM cache','PSRAM -> NOR / hardware','Whole-NOR compare / hardware','PASS / whole NOR matched')){
 if(!$appText.Contains($marker)){throw "Missing application marker: $marker"}
}
Copy-Item -LiteralPath $baseline -Destination $bundle -Recurse
Remove-Item -LiteralPath "$bundle/DEVELOPMENT_SOURCE" -Recurse -Force -ErrorAction SilentlyContinue
$core="$bundle/SD_ROOT/Cores/ChisBurner.ChisBurner"
$assets="$bundle/SD_ROOT/Assets/chisburner/common"
if(-not('ChisBurner.Stream131.Reverse'-as[type])){Add-Type -TypeDefinition @'
namespace ChisBurner.Stream131 { public static class Reverse { public static byte[] Apply(byte[] a) { var t=new byte[256]; for(int i=0;i<256;i++){int x=i,y=0;for(int b=0;b<8;b++){y=(y<<1)|(x&1);x>>=1;}t[i]=(byte)y;}var r=new byte[a.Length];for(int i=0;i<a.Length;i++)r[i]=t[a[i]];return r; } } }
'@}
[IO.File]::WriteAllBytes("$root/$core/chis.rbf_r",[ChisBurner.Stream131.Reverse]::Apply([IO.File]::ReadAllBytes("$root/$fpga/output_files/ap_core.rbf")))
Copy-Item "$os/os.bin" "$assets/os.bin" -Force
Copy-Item $app "$assets/cb102.elf" -Force
$utf8=[Text.UTF8Encoding]::new($false)
$meta=Get-Content "$core/core.json" -Raw|ConvertFrom-Json
$meta.core.metadata.description='TF to dedicated dual PSRAM, dynamic NOR erase/program, hardware whole-NOR comparison.'
$meta.core.metadata.version='0.1.0-smoke.131-hw-psram-nor-r1'
$meta.core.metadata.date_release='2026-09-14'
[IO.File]::WriteAllText("$root/$core/core.json",($meta|ConvertTo-Json -Depth 20),$utf8)
[IO.File]::WriteAllText("$root/$core/info.txt",'smoke.131 destructive hardware PSRAM-to-NOR test. Read TEST_CN.md before use.',$utf8)
Copy-Item docs/SMOKE131_TEST_CN.md "$bundle/TEST_CN.md" -Force
Copy-Item docs/STREAM131_SERVICE_CHECKPOINT.md "$bundle/DEVELOPMENT_NOTES.md" -Force
Remove-Item "$bundle/README.txt","$bundle/VERIFICATION.md" -Force -ErrorAction SilentlyContinue
[IO.File]::WriteAllText("$root/$bundle/README.txt",'ChisBurner smoke.131 destructive hardware PSRAM-to-NOR test package. Copy SD_ROOT to the Pocket SD card and read TEST_CN.md before use.',$utf8)
$receipt=@()
foreach($p in @("$os/firmware.elf","$os/firmware.mif","$os/os.bin",$app,"$fpga/output_files/ap_core.rbf","$fpga/ap_core.qsf","$fpga/full-compile.log","$fpga/cdc-report.log",'build/stream131-release/sections.txt','build/stream131-release/symbols.txt')){$receipt+="$(Sha $p)  $p"}
[IO.File]::WriteAllLines("$root/$bundle/BUILD_INPUTS_SHA256.txt",[string[]]$receipt,$utf8)
$sources=@('app/ui98/nor130_transfer.h','app/ui98/transfer.c','app/ui98/Makefile','app/ui98/version.h','core/nor/stream131.c','core/nor/submit131.c','core/rtl/chisburner_cart_auto108.v','.integration/openfpgaos/src/fpga/common/axi_periph_slave.v','.integration/openfpgaos/src/fpga/targets/pocket/core_top.v','.integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_lifecycle.v','.integration/openfpgaos/src/firmware/api/of_services.h','.integration/openfpgaos/src/firmware/api/of_nor.h','.integration/openfpgaos/src/firmware/os/kernel/services_table.c','.integration/openfpgaos/src/firmware/os/kernel/nor108.c','tests/build-stream131-os.sh','tests/build-stream131-fpga.ps1','tests/configure-stream131.tcl','tests/build-stream131-package.ps1','docs/SMOKE131_TEST_CN.md','docs/STREAM131_SERVICE_CHECKPOINT.md')
$sources+=@(Get-ChildItem core/rtl/chisburner_*131.v -File|ForEach-Object{$_.FullName.Substring($root.Length+1).Replace('\','/')})
[IO.File]::WriteAllLines("$root/$bundle/SOURCES_SHA256.txt",[string[]]@($sources|ForEach-Object{"$(Sha $_)  $_"}),$utf8)
$prefix=[IO.Path]::GetFullPath("$bundle/SD_ROOT").TrimEnd('\')+'\'
$sums=@(Get-ChildItem "$bundle/SD_ROOT" -Recurse -File|Sort-Object FullName|ForEach-Object{"$(Sha $_.FullName)  $($_.FullName.Substring($prefix.Length).Replace('\','/'))"})
[IO.File]::WriteAllLines("$root/$bundle/SHA256SUMS.txt",[string[]]$sums,$utf8)
Compress-Archive -Path "$bundle/*" -DestinationPath "$bundle.zip"
Add-Type -AssemblyName System.IO.Compression.FileSystem
$zip=[IO.Compression.ZipFile]::OpenRead("$root/$bundle.zip")
try {
 foreach($line in $sums){
  $hash,$path=$line -split '  ',2;$entry=$zip.GetEntry("SD_ROOT/$path")
  if(!$entry){throw "Missing ZIP entry $path"}
  $stream=$entry.Open();$sha=[Security.Cryptography.SHA256]::Create()
  try{$actual=([BitConverter]::ToString($sha.ComputeHash($stream))).Replace('-','').ToLowerInvariant()}finally{$stream.Dispose();$sha.Dispose()}
  if($actual -cne $hash){throw "ZIP hash mismatch $path"}
 }
}finally{$zip.Dispose()}
Get-FileHash "$bundle.zip" -Algorithm SHA256
