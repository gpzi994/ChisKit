$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$base="$root/build/smoke/ChisBurner-dev.169-rtc-footer-r1"
$dst="$root/build/smoke/ChisBurner-dev.170-flash-write-r1"
$fpga="$root/build/flash170-quartus"
$os="$root/.integration/openfpgaos/src/firmware/os/bld/flash170"
function Sha($p){(Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash}
if((Test-Path $dst)-or(Test-Path "$dst.zip")){throw 'Do not overwrite package'}
foreach($line in Get-Content "$base/SHA256SUMS.txt"){
 $hash,$p=$line -split '  ',2
 if((Sha "$base/SD_ROOT/$p") -ne $hash){throw "Base mismatch $p"}
}
if((Sha "$fpga/firmware.mif") -ne (Sha "$os/firmware.mif")){throw 'OS/FPGA MIF mismatch'}
foreach($f in @('mif-update-170.log','asm-170.log')){
 if((Get-Content "$fpga/$f" -Raw) -notmatch 'successful'){throw "Unverified $f"}
}
& "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $fpga -AllowUnusedBridgeReady
. "$PSScriptRoot/os-image-integrity.ps1"
$null=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes("$os/os.bin"))
$app="$root/build/flash170-app/app.elf"
$strings=[Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($app))
foreach($s in @('dev.170-flash-write','Write Flash SAV','Backup Flash 128 KiB','Backup Flash 64 KiB','Write EEPROM SAV')){
 if(!$strings.Contains($s)){throw "Missing app string $s"}
}
$null=New-Item -ItemType Directory -Path $dst
Copy-Item "$base/SD_ROOT" "$dst/SD_ROOT" -Recurse
Copy-Item $app "$dst/SD_ROOT/Assets/chisburner/common/cb102.elf"
Copy-Item "$os/os.bin" "$dst/SD_ROOT/Assets/chisburner/common/os.bin"
Add-Type -TypeDefinition 'public static class Flash170Reverse {public static byte[] Run(byte[] a){for(int i=0;i<a.Length;i++){int x=a[i],y=0;for(int k=0;k<8;k++){y=(y<<1)|(x&1);x>>=1;}a[i]=(byte)y;}return a;}}'
[IO.File]::WriteAllBytes("$dst/SD_ROOT/Cores/ChisBurner.ChisBurner/chis.rbf_r",[Flash170Reverse]::Run([IO.File]::ReadAllBytes("$fpga/output_files/ap_core.rbf")))
$metaPath="$dst/SD_ROOT/Cores/ChisBurner.ChisBurner/core.json"
$meta=Get-Content $metaPath -Raw|ConvertFrom-Json
$meta.core.metadata.version='0.1.0-dev.170-flash-write-r1'
$utf8=[Text.UTF8Encoding]::new($false)
[IO.File]::WriteAllText($metaPath,($meta|ConvertTo-Json -Depth 20),$utf8)
Copy-Item "$root/docs/FLASH170_TEST_CN.md" "$dst/TEST_CN.md"
Copy-Item "$root/build/flash170-build.log" "$dst/BUILD.log"
Copy-Item "$root/build/flash170-regression.log" "$dst/REGRESSION.log"
Copy-Item "$base/ThirdParty" "$dst/ThirdParty" -Recurse
$null=New-Item -ItemType Directory -Path "$dst/Source/app","$dst/Source/OS","$dst/Source/tests" -Force
foreach($d in @('ui98','ui97','gba_db55')){Copy-Item "$root/app/$d" "$dst/Source/app/$d" -Recurse}
foreach($f in @('flash170.h','flash166.h','save139.h','sram138.h','nor108.c','nor108.h','services_table.c')){
 Copy-Item "$root/.integration/openfpgaos/src/firmware/os/kernel/$f" "$dst/Source/OS/$f"
}
Copy-Item "$root/.integration/openfpgaos/src/firmware/api/of_cart.h","$root/.integration/openfpgaos/src/firmware/api/of_services.h","$root/.integration/openfpgaos/src/firmware/api/of_flash170.h" "$dst/Source/OS/"
Copy-Item "$root/core/save/flash170.h" "$dst/Source/OS/flash170-algorithm.h"
Copy-Item "$root/build/flash170-os-build.log" "$dst/OS_BUILD.log"
Copy-Item "$os/firmware.mif" "$dst/Source/OS/firmware.mif"
foreach($f in @('test-flash170.c','test-flash170-os.c','test-flash165.c','test-eeprom162-ui.c','check-flash170.sh','build-flash170-fpga.ps1','package-flash170.ps1')){
 Copy-Item "$root/tests/$f" "$dst/Source/tests/$f"
}
foreach($f in @('mif-update-170.log','asm-170.log','sta-170.log','unconstrained.txt')){Copy-Item "$fpga/$f" "$dst/$f"}
$prefix=[IO.Path]::GetFullPath("$dst/SD_ROOT")+'\'
$sums=@(Get-ChildItem "$dst/SD_ROOT" -Recurse -File|Sort-Object FullName|ForEach-Object{"$(Sha $_.FullName)  $($_.FullName.Substring($prefix.Length).Replace('\','/'))"})
[IO.File]::WriteAllLines("$dst/SHA256SUMS.txt",[string[]]$sums,$utf8)
Compress-Archive -Path "$dst/*" -DestinationPath "$dst.zip"
Write-Host "PASS $dst.zip"
Get-FileHash "$dst.zip"
