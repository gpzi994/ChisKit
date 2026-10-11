param([switch]$R2,[switch]$Sram138)
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$fpga="$root/build/sram137-quartus"
$os="$root/.integration/openfpgaos/src/firmware/os/bld/sram137-release"
$app="$root/build/sram137-app/app.elf"
$dst="$root/build/smoke/ChisBurner-smoke.137-sram"
$version='dev.137-sram'
if($R2){$app="$root/build/sram137-r2-app/app.elf";$dst="$root/build/smoke/ChisBurner-smoke.137-r2";$version='dev.137-r2'}
if($Sram138){
 if($R2){throw 'Select exactly one version'}
 $fpga="$root/build/sram138-quartus"
 $os="$root/.integration/openfpgaos/src/firmware/os/bld/sram138-release"
 $app="$root/build/sram138-app/app.elf"
 $dst="$root/build/smoke/ChisBurner-smoke.138-save"
 $version='dev.138-sav'
}
function Sha($p){(Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash.ToLowerInvariant()}
if((Test-Path $dst)-or(Test-Path "$dst.zip")){throw 'Preserve previous package'}
if((Sha "$os/firmware.mif") -cne (Sha "$fpga/firmware.mif")){throw 'MIF mismatch'}
if((Get-Content "$fpga/full-compile.log" -Raw) -notmatch 'Full Compilation was successful'){throw 'Compile not successful'}
& "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $fpga
. "$PSScriptRoot/os-image-integrity.ps1"
$null=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes("$os/os.bin"))
if(!([Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($app))).Contains($version)){throw 'Wrong app version'}
$null=New-Item -ItemType Directory -Path $dst
Copy-Item -LiteralPath "$root/build/smoke/ChisBurner-smoke.135-timers/SD_ROOT" -Destination "$dst/SD_ROOT" -Recurse
$core="$dst/SD_ROOT/Cores/ChisBurner.ChisBurner"
$assets="$dst/SD_ROOT/Assets/chisburner/common"
Add-Type -TypeDefinition @'
public static class Sram137Bits { public static byte[] Reverse(byte[] a) { for(int i=0;i<a.Length;i++){int x=a[i],y=0;for(int b=0;b<8;b++){y=(y<<1)|(x&1);x>>=1;}a[i]=(byte)y;}return a;} }
'@
[IO.File]::WriteAllBytes("$core/chis.rbf_r",[Sram137Bits]::Reverse([IO.File]::ReadAllBytes("$fpga/output_files/ap_core.rbf")))
Copy-Item -LiteralPath "$os/os.bin" -Destination "$assets/os.bin" -Force
Copy-Item -LiteralPath $app -Destination "$assets/cb102.elf" -Force
$utf8=[Text.UTF8Encoding]::new($false)
$meta=Get-Content "$core/core.json" -Raw|ConvertFrom-Json
$meta.core.metadata.version='0.1.0-smoke.137-sram'
if($R2){$meta.core.metadata.version='0.1.0-smoke.137-r2'}
if($Sram138){$meta.core.metadata.version='0.1.0-smoke.138-save'}
[IO.File]::WriteAllText("$core/core.json",($meta|ConvertTo-Json -Depth 20),$utf8)
$data=Get-Content "$core/data.json" -Raw|ConvertFrom-Json
$slot=@($data.data.data_slots|Where-Object {$_.id -eq 5})
if($slot.Count -ne 1){throw 'Output slot missing'}
$slot[0].extensions=@('gba','gb','gbc','sav')
[IO.File]::WriteAllText("$core/data.json",($data|ConvertTo-Json -Depth 20),$utf8)
Copy-Item "$root/docs/SMOKE137_SRAM_TEST_CN.md" "$dst/TEST_CN.md"
if($R2){Copy-Item "$root/docs/SMOKE137_R2.md" "$dst/TEST_CN.md" -Force}
Copy-Item "$PSScriptRoot/verify-sram137.ps1" "$dst/VERIFY_SAVE.ps1"
if($Sram138){
 Copy-Item "$root/docs/SMOKE138_SAVE_TEST_CN.md" "$dst/TEST_CN.md" -Force
 Copy-Item "$PSScriptRoot/verify-sram138.ps1" "$dst/VERIFY_SAVE.ps1" -Force
}
Copy-Item "$fpga/output_files/ap_core.fit.summary" "$dst/FIT_SUMMARY.txt"
Copy-Item "$fpga/output_files/ap_core.sta.summary" "$dst/TIMING_SUMMARY.txt"
[IO.File]::WriteAllText("$dst/README.txt","${version}: install COMPLETE SD_ROOT. SRAM/FRAM 32 KiB backup only; SAVE writes/RTC disabled. Read TEST_CN.md. Hardware validation pending. PSRAM stays ROM-only.`r`n",$utf8)
if($Sram138){[IO.File]::WriteAllText("$dst/README.txt","${version}: install COMPLETE SD_ROOT. Parallel SRAM/FRAM backup test: Retail32 or ChisFlash128 profile in Options. SAVE writes, Flash-save ID, EEPROM and RTC NOT supported. Read TEST_CN.md. Hardware validation pending. All save buffers SDRAM; PSRAM stays ROM-only.`r`n",$utf8)}
$inputs=@($app,"$os/os.bin","$os/firmware.mif","$fpga/output_files/ap_core.rbf","$fpga/ap_core.qsf","$fpga/full-compile.log")
$inputs+=@(Get-ChildItem "$root/core/save","$root/app/ui98" -File | Select-Object -ExpandProperty FullName)
$inputs+=@("$root/core/rtl/chisburner_nor_bus108.v","$root/core/rtl/chisburner_cart_auto98.v","$root/.integration/openfpgaos/src/firmware/os/kernel/nor108.c","$root/.integration/openfpgaos/src/firmware/os/kernel/services_table.c")
if($Sram138){$inputs+=@("$root/.integration/openfpgaos/src/firmware/os/kernel/sram138.h","$root/.integration/openfpgaos/src/firmware/api/of_cart.h")}
[IO.File]::WriteAllLines("$dst/BUILD_INPUTS_SHA256.txt",[string[]]@($inputs|ForEach-Object{"$(Sha $_)  $_"}),$utf8)
$prefix="$dst/SD_ROOT/"
$sums=@(Get-ChildItem "$dst/SD_ROOT" -Recurse -File|Sort-Object FullName|ForEach-Object{"$(Sha $_.FullName)  $($_.FullName.Substring($prefix.Length).Replace('\','/'))"})
[IO.File]::WriteAllLines("$dst/SHA256SUMS.txt",[string[]]$sums,$utf8)
Compress-Archive -Path "$dst/*" -DestinationPath "$dst.zip"
Add-Type -AssemblyName System.IO.Compression.FileSystem
$z=[IO.Compression.ZipFile]::OpenRead("$dst.zip")
try {foreach($line in $sums){
 $h,$f=$line -split '  ',2
 $e=$z.GetEntry("SD_ROOT/$f")
 if(!$e){$e=$z.GetEntry("SD_ROOT\$($f.Replace('/','\'))")}
 if(!$e){throw "Missing $f"}
 $s=$e.Open();$a=[Security.Cryptography.SHA256]::Create()
 try{$actual=([BitConverter]::ToString($a.ComputeHash($s))).Replace('-','').ToLowerInvariant()}finally{$s.Dispose();$a.Dispose()}
 if($actual -cne $h){throw "ZIP mismatch $f"}
}}finally{$z.Dispose()}
'PASS ZIP hashes'
Get-FileHash "$dst.zip"
