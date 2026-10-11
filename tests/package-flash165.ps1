param([ValidateSet('r1','r2','r3')][string]$Revision='r3')
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$base=Join-Path $root 'build/smoke/ChisBurner-dev.164-rom-r1'
$dst=Join-Path $root "build/smoke/ChisBurner-dev.165-flash64-$Revision"
function Sha($p){(Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash}
if((Test-Path $dst)-or(Test-Path "$dst.zip")){throw 'Do not overwrite package'}
foreach($line in Get-Content "$base/SHA256SUMS.txt"){
 $hash,$p=$line -split '  ',2
 if((Sha "$base/SD_ROOT/$p") -ne $hash){throw "Base mismatch $p"}
}
$build=if($Revision -ne 'r1'){'flash165-r2'}else{'flash165'}
$app="$root/build/$build-app/app.elf"
$strings=[Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($app))
foreach($s in @($(if($Revision -ne 'r1'){'dev.165-f64-r2'}else{'dev.165-flash64'}),'Backup Flash 64 KiB','NOR capacity backup','Write EEPROM SAV')){
 if(!$strings.Contains($s)){throw "Missing app string $s"}
}
$null=New-Item -ItemType Directory -Path $dst
Copy-Item "$base/SD_ROOT" "$dst/SD_ROOT" -Recurse
Copy-Item $app "$dst/SD_ROOT/Assets/chisburner/common/cb102.elf"
$metaPath="$dst/SD_ROOT/Cores/ChisBurner.ChisBurner/core.json"
$meta=Get-Content $metaPath -Raw|ConvertFrom-Json
$meta.core.metadata.version="0.1.0-dev.165-flash64-$Revision"
$utf8=[Text.UTF8Encoding]::new($false)
[IO.File]::WriteAllText($metaPath,($meta|ConvertTo-Json -Depth 20),$utf8)
foreach($p in @('Assets/chisburner/common/os.bin','Cores/ChisBurner.ChisBurner/chis.rbf_r')){
 if((Sha "$base/SD_ROOT/$p") -ne (Sha "$dst/SD_ROOT/$p")){throw "Firmware changed $p"}
}
if($Revision -ne 'r1'){
 Copy-Item "$root/.integration/openfpgaos/src/firmware/os/bld/eeprom162/os.bin" "$dst/SD_ROOT/Assets/chisburner/common/os.bin"
 Copy-Item "$root/build/flash165-r2-os.log" "$dst/OS_BUILD.log"
 $null=New-Item -ItemType Directory -Path "$dst/Source/OS" -Force
 foreach($f in @('sram138.h','nor108.h','services_table.c')){Copy-Item "$root/.integration/openfpgaos/src/firmware/os/kernel/$f" "$dst/Source/OS/$f"}
 Copy-Item "$root/.integration/openfpgaos/src/firmware/api/of_cart.h" "$dst/Source/OS/of_cart.h"
 Copy-Item "$root/tests/test-sram138-os.c" "$dst/Source/OS/test-sram138-os.c"
}
if($Revision -eq 'r3'){
 $fpga="$root/build/flash165-r3-quartus"
 if((Sha "$fpga/firmware.mif") -ne (Sha "$root/.integration/openfpgaos/src/firmware/os/bld/eeprom162/firmware.mif")){throw 'OS/FPGA MIF mismatch'}
 if((Get-Content "$fpga/mif-update-r3.log" -Raw) -notmatch 'successful'){throw 'MIF update not verified'}
 if((Get-Content "$fpga/asm-r3.log" -Raw) -notmatch 'successful'){throw 'Assembly not verified'}
 & "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $fpga -AllowUnusedBridgeReady
 . "$PSScriptRoot/os-image-integrity.ps1"
 $null=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes("$dst/SD_ROOT/Assets/chisburner/common/os.bin"))
 Add-Type -TypeDefinition 'public static class Flash165Reverse {public static byte[] Run(byte[] a){for(int i=0;i<a.Length;i++){int x=a[i],y=0;for(int k=0;k<8;k++){y=(y<<1)|(x&1);x>>=1;}a[i]=(byte)y;}return a;}}'
 [IO.File]::WriteAllBytes("$dst/SD_ROOT/Cores/ChisBurner.ChisBurner/chis.rbf_r",[Flash165Reverse]::Run([IO.File]::ReadAllBytes("$fpga/output_files/ap_core.rbf")))
 Copy-Item "$fpga/firmware.mif" "$dst/Source/OS/firmware.mif"
 foreach($f in @('mif-update-r3.log','asm-r3.log','sta-r3.log','unconstrained.txt')){Copy-Item "$fpga/$f" "$dst/$f"}
 Copy-Item "$PSScriptRoot/build-flash165-r3.ps1" "$dst/Source/OS/build-flash165-r3.ps1"
}
Copy-Item "$root/docs/FLASH165_TEST_CN.md" "$dst/TEST_CN.md"
Copy-Item "$root/build/$build-build.log" "$dst/BUILD.log"
Copy-Item "$root/build/$build-app/sources.sha256" "$dst/SOURCES_SHA256.txt"
Copy-Item "$base/ThirdParty" "$dst/ThirdParty" -Recurse
$null=New-Item -ItemType Directory -Path "$dst/Source/app","$dst/Source/tests" -Force
foreach($d in @('ui98','ui97','gba_db55')){Copy-Item "$root/app/$d" "$dst/Source/app/$d" -Recurse}
foreach($f in @('test-flash165.c','check-flash165.sh','test-eeprom162-ui.c','package-flash165.ps1')){Copy-Item "$root/tests/$f" "$dst/Source/tests/$f"}
$prefix=[IO.Path]::GetFullPath("$dst/SD_ROOT")+'\'
$sums=@(Get-ChildItem "$dst/SD_ROOT" -Recurse -File|Sort-Object FullName|ForEach-Object{"$(Sha $_.FullName)  $($_.FullName.Substring($prefix.Length).Replace('\','/'))"})
[IO.File]::WriteAllLines("$dst/SHA256SUMS.txt",[string[]]$sums,$utf8)
Compress-Archive -Path "$dst/*" -DestinationPath "$dst.zip"
Write-Host "PASS $dst.zip"
Get-FileHash "$dst.zip"
