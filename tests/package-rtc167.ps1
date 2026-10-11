$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$base="$root/build/smoke/ChisBurner-dev.166-flash128-r1"
$dst="$root/build/smoke/ChisBurner-dev.167-rtc-read-r2"
$fpga="$root/build/rtc167-r2-quartus"
$os="$root/.integration/openfpgaos/src/firmware/os/bld/rtc167-r2"
function Sha($p){(Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash}
if((Test-Path $dst)-or(Test-Path "$dst.zip")){throw 'Do not overwrite package'}
foreach($line in Get-Content "$base/SHA256SUMS.txt"){
 $hash,$p=$line -split '  ',2
 if((Sha "$base/SD_ROOT/$p") -ne $hash){throw "Base mismatch $p"}
}
if((Sha "$fpga/firmware.mif") -ne (Sha "$os/firmware.mif")){throw 'OS/FPGA MIF mismatch'}
foreach($f in @('mif-update-167.log','asm-167.log')){
 if((Get-Content "$fpga/$f" -Raw) -notmatch 'successful'){throw "Unverified $f"}
}
& "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $fpga -AllowUnusedBridgeReady
. "$PSScriptRoot/os-image-integrity.ps1"
$null=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes("$os/os.bin"))
$app="$root/build/rtc167-r2-app/app.elf"
$strings=[Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($app))
foreach($s in @('dev.167-rtc-r2','Backup Flash 128 KiB','Backup Flash 64 KiB','Write EEPROM SAV')){
 if(!$strings.Contains($s)){throw "Missing app string $s"}
}
$null=New-Item -ItemType Directory -Path $dst
Copy-Item "$base/SD_ROOT" "$dst/SD_ROOT" -Recurse
Copy-Item $app "$dst/SD_ROOT/Assets/chisburner/common/cb102.elf"
Copy-Item "$os/os.bin" "$dst/SD_ROOT/Assets/chisburner/common/os.bin"
Add-Type -TypeDefinition 'public static class Rtc167Reverse {public static byte[] Run(byte[] a){for(int i=0;i<a.Length;i++){int x=a[i],y=0;for(int k=0;k<8;k++){y=(y<<1)|(x&1);x>>=1;}a[i]=(byte)y;}return a;}}'
[IO.File]::WriteAllBytes("$dst/SD_ROOT/Cores/ChisBurner.ChisBurner/chis.rbf_r",[Rtc167Reverse]::Run([IO.File]::ReadAllBytes("$fpga/output_files/ap_core.rbf")))
$metaPath="$dst/SD_ROOT/Cores/ChisBurner.ChisBurner/core.json"
$meta=Get-Content $metaPath -Raw|ConvertFrom-Json
$meta.core.metadata.version='0.1.0-dev.167-rtc-read-r2'
$utf8=[Text.UTF8Encoding]::new($false)
[IO.File]::WriteAllText($metaPath,($meta|ConvertTo-Json -Depth 20),$utf8)
Copy-Item "$root/docs/RTC167_TEST_CN.md" "$dst/TEST_CN.md"
Copy-Item "$root/build/rtc167-r2-build.log" "$dst/BUILD.log"
Copy-Item "$root/build/rtc167-r2-regression.log" "$dst/REGRESSION.log"
Copy-Item "$base/ThirdParty" "$dst/ThirdParty" -Recurse
$null=New-Item -ItemType Directory -Path "$dst/Source/app","$dst/Source/OS","$dst/Source/tests" -Force
foreach($d in @('ui98','ui97','gba_db55')){Copy-Item "$root/app/$d" "$dst/Source/app/$d" -Recurse}
foreach($f in @('rtc167.h','flash166.h','save139.h','sram138.h','nor108.c','nor108.h','services_table.c')){
 Copy-Item "$root/.integration/openfpgaos/src/firmware/os/kernel/$f" "$dst/Source/OS/$f"
}
Copy-Item "$root/.integration/openfpgaos/src/firmware/api/of_cart.h" "$dst/Source/OS/of_cart.h"
foreach($f in @('of_rtc167.h','of_rtc167_data.h')){Copy-Item "$root/.integration/openfpgaos/src/firmware/api/$f" "$dst/Source/OS/$f"}
Copy-Item "$root/core/rtc" "$dst/Source/rtc" -Recurse
Copy-Item "$os/firmware.mif" "$dst/Source/OS/firmware.mif"
foreach($f in @('test-rtc167-os.c','test-rtc167.c','test-flash165.c','test-eeprom162-ui.c','build-rtc167.sh','build-rtc167-fpga.ps1','package-rtc167.ps1')){
 Copy-Item "$root/tests/$f" "$dst/Source/tests/$f"
}
foreach($f in @('mif-update-167.log','asm-167.log','sta-167.log','unconstrained.txt')){Copy-Item "$fpga/$f" "$dst/$f"}
$prefix=[IO.Path]::GetFullPath("$dst/SD_ROOT")+'\'
$sums=@(Get-ChildItem "$dst/SD_ROOT" -Recurse -File|Sort-Object FullName|ForEach-Object{"$(Sha $_.FullName)  $($_.FullName.Substring($prefix.Length).Replace('\','/'))"})
[IO.File]::WriteAllLines("$dst/SHA256SUMS.txt",[string[]]$sums,$utf8)
Compress-Archive -Path "$dst/*" -DestinationPath "$dst.zip"
Write-Host "PASS $dst.zip"
Get-FileHash "$dst.zip"
