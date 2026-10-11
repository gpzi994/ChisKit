$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$target="$root/build/smoke/ChisBurner-smoke.131-hw-psram-nor-r8"
$fpga="$root/build/stream131-r8-timing2-quartus"
$os="$root/.integration/openfpgaos/src/firmware/os/bld/stream131-r8-release"
$app="$root/build/ui131-r8-app/app.elf"
function Sha($p){(Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash.ToLowerInvariant()}
if((Test-Path $target)-or(Test-Path "$target.zip")){throw 'Preserve existing r8'}
if((Sha "$os/firmware.mif") -cne (Sha "$fpga/firmware.mif")){throw 'MIF mismatch'}
if((Get-Content "$fpga/r8-full-compile.log" -Raw) -notmatch 'Full Compilation was successful'){throw 'Full r8 compile not successful'}
& "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $fpga
. "$PSScriptRoot/os-image-integrity.ps1"
$null=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes("$os/os.bin"))
if(!([Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($app))).Contains('smoke.131-r8')){throw 'Wrong app'}
Copy-Item -LiteralPath "$root/build/smoke/ChisBurner-smoke.131-hw-psram-nor-r7" -Destination $target -Recurse
$core="$target/SD_ROOT/Cores/ChisBurner.ChisBurner"
$assets="$target/SD_ROOT/Assets/chisburner/common"
Add-Type -TypeDefinition @'
public static class Stream131R8Bits { public static byte[] Reverse(byte[] a) { for(int i=0;i<a.Length;i++){int x=a[i],y=0;for(int b=0;b<8;b++){y=(y<<1)|(x&1);x>>=1;}a[i]=(byte)y;}return a;} }
'@
[IO.File]::WriteAllBytes("$core/chis.rbf_r",[Stream131R8Bits]::Reverse([IO.File]::ReadAllBytes("$fpga/output_files/ap_core.rbf")))
Copy-Item "$os/os.bin" "$assets/os.bin" -Force
Copy-Item $app "$assets/cb102.elf" -Force
$utf8=[Text.UTF8Encoding]::new($false)
$meta=Get-Content "$core/core.json" -Raw|ConvertFrom-Json
$meta.core.metadata.version='0.1.0-smoke.131-hw-psram-nor-r8'
[IO.File]::WriteAllText("$core/core.json",($meta|ConvertTo-Json -Depth 20),$utf8)
Copy-Item "$root/docs/SMOKE131_R8_TEST_CN.md" "$target/R8_TEST_CN.md"
Copy-Item "$root/docs/SMOKE131_R8_TEST_CN.md" "$target/TEST_CN.md" -Force
[IO.File]::WriteAllText("$target/README.txt","ChisBurner smoke.131-r8`r`nInstall the COMPLETE SD_ROOT contents. Read R8_TEST_CN.md first.`r`nFPGA, boot firmware, OS and application are rebuilt together. Hardware testing is pending.`r`nPSRAM0/1 remain ROM-only; no new PSRAM workspace or save/config use.`r`nR8_BUILD_INPUTS_SHA256.txt and SHA256SUMS.txt describe this package. Other release notes and receipts are historical.`r`n",$utf8)
$receipt=@('r8 full FPGA rebuild, boot MIF, OS and application. Older receipts are historical only. Hardware qualification pending.')
foreach($p in @($app,"$os/os.bin","$os/firmware.mif","$fpga/output_files/ap_core.rbf","$fpga/r8-full-compile.log","$root/core/rtl/chisburner_nor_program131.v","$root/core/nor/stream131.c","$root/core/nor/recipe.c","$root/core/nor/mori/chisburner_nor_db_data.c","$root/.integration/openfpgaos/src/firmware/os/kernel/nor108.c")){$receipt+="$(Sha $p)  $p"}
[IO.File]::WriteAllLines("$target/R8_BUILD_INPUTS_SHA256.txt",[string[]]$receipt,$utf8)
$periph="$root/.integration/openfpgaos/src/fpga/common/axi_periph_slave.v"
[IO.File]::AppendAllText("$target/R8_BUILD_INPUTS_SHA256.txt","$(Sha $periph)  $periph`n",$utf8)
$prefix="$target/SD_ROOT/"
$sums=@(Get-ChildItem "$target/SD_ROOT" -Recurse -File|Sort-Object FullName|ForEach-Object{"$(Sha $_.FullName)  $($_.FullName.Substring($prefix.Length).Replace('\','/'))"})
[IO.File]::WriteAllLines("$target/SHA256SUMS.txt",[string[]]$sums,$utf8)
Compress-Archive -Path "$target/*" -DestinationPath "$target.zip"
Add-Type -AssemblyName System.IO.Compression.FileSystem
$z=[IO.Compression.ZipFile]::OpenRead("$target.zip")
try {foreach($line in $sums){
 $h,$f=$line -split '  ',2;$e=$z.GetEntry("SD_ROOT/$f")
 if(!$e){$e=$z.GetEntry("SD_ROOT\$($f.Replace('/','\'))")};if(!$e){throw "Missing $f"}
 $s=$e.Open();$a=[Security.Cryptography.SHA256]::Create()
 try{$actual=([BitConverter]::ToString($a.ComputeHash($s))).Replace('-','').ToLowerInvariant()}finally{$s.Dispose();$a.Dispose()}
 if($actual -cne $h){throw "Mismatch $f"}
}}finally{$z.Dispose()}
'PASS ZIP SD_ROOT hashes'
Get-FileHash "$target.zip"
