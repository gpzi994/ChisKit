$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$base="$root/build/smoke/ChisBurner-dev.170-flash-write-r3"
$dst="$root/build/smoke/ChisBurner-dev.171-blank-save-r3"
$fpga="$root/build/save171-r3-quartus"
$os="$root/.integration/openfpgaos/src/firmware/os/bld/save171"
function Sha($p){(Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash}
if((Test-Path $dst)-or(Test-Path "$dst.zip")){throw 'Preserve existing package'}
foreach($line in Get-Content "$base/SHA256SUMS.txt"){
 $hash,$p=$line -split '  ',2
 if((Sha "$base/SD_ROOT/$p") -ne $hash){throw "Base mismatch $p"}
}
if((Sha "$fpga/firmware.mif") -ne (Sha "$os/firmware.mif")){throw 'MIF mismatch'}
foreach($f in @('mif-update-171.log','asm-171.log')){
 if((Get-Content "$fpga/$f" -Raw) -notmatch 'successful'){throw "Unverified $f"}
}
& "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $fpga -AllowUnusedBridgeReady
. "$PSScriptRoot/os-image-integrity.ps1"
$null=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes("$os/os.bin"))
$app="$root/build/save171-r3-app/app.elf"
$strings=[Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($app))
foreach($s in @('dev.171-r3','Write selected SAV','Save restored and fully verified','Backup Flash 128 KiB')){
 if(!$strings.Contains($s)){throw "Missing app string $s"}
}
$null=New-Item -ItemType Directory -Path $dst
Copy-Item "$base/SD_ROOT" "$dst/SD_ROOT" -Recurse
Copy-Item $app "$dst/SD_ROOT/Assets/chisburner/common/cb102.elf"
Copy-Item "$os/os.bin" "$dst/SD_ROOT/Assets/chisburner/common/os.bin"
Add-Type -TypeDefinition 'public static class Save171Reverse {public static byte[] Run(byte[] a){for(int i=0;i<a.Length;i++){int x=a[i],y=0;for(int k=0;k<8;k++){y=(y<<1)|(x&1);x>>=1;}a[i]=(byte)y;}return a;}}'
[IO.File]::WriteAllBytes("$dst/SD_ROOT/Cores/ChisBurner.ChisBurner/chis.rbf_r",[Save171Reverse]::Run([IO.File]::ReadAllBytes("$fpga/output_files/ap_core.rbf")))
$metaPath="$dst/SD_ROOT/Cores/ChisBurner.ChisBurner/core.json"
$meta=Get-Content $metaPath -Raw|ConvertFrom-Json
$meta.core.metadata.version='0.1.0-dev.171-blank-save-r3'
$utf8=[Text.UTF8Encoding]::new($false)
[IO.File]::WriteAllText($metaPath,($meta|ConvertTo-Json -Depth 20),$utf8)
Copy-Item "$root/docs/SAVE171_TEST_CN.md" "$dst/TEST_CN.md"
Copy-Item "$root/build/save171-build.log" "$dst/BUILD.log"
Copy-Item "$root/build/save171-os-build.log" "$dst/OS_BUILD.log"
Copy-Item "$root/build/save171-regression.log" "$dst/REGRESSION.log"
Copy-Item "$base/ThirdParty" "$dst/ThirdParty" -Recurse
$null=New-Item -ItemType Directory -Path "$dst/Source/app","$dst/Source/OS","$dst/Source/tests","$dst/Source/core"
foreach($d in @('ui98','ui97','gba_db55')){Copy-Item "$root/app/$d" "$dst/Source/app/$d" -Recurse}
Copy-Item "$root/core/save" "$dst/Source/core/save" -Recurse
Copy-Item "$root/.integration/openfpgaos/src/firmware/os/kernel" "$dst/Source/OS/kernel" -Recurse
Copy-Item "$root/.integration/openfpgaos/src/firmware/api" "$dst/Source/OS/api" -Recurse
Copy-Item "$os/firmware.mif" "$dst/Source/OS/firmware.mif"
Get-ChildItem "$root/tests" -File | Where-Object Name -Match 'save171|eeprom162-service' | Copy-Item -Destination "$dst/Source/tests"
foreach($f in @('mif-update-171.log','asm-171.log','sta-171.log','unconstrained.txt')){Copy-Item "$fpga/$f" "$dst/$f"}
$prefix=[IO.Path]::GetFullPath("$dst/SD_ROOT")+'\'
$sums=@(Get-ChildItem "$dst/SD_ROOT" -Recurse -File|Sort-Object FullName|ForEach-Object{"$(Sha $_.FullName)  $($_.FullName.Substring($prefix.Length).Replace('\','/'))"})
[IO.File]::WriteAllLines("$dst/SHA256SUMS.txt",[string[]]$sums,$utf8)
Compress-Archive -Path "$dst/*" -DestinationPath "$dst.zip"
Write-Host "PASS $dst.zip"
Get-FileHash "$dst.zip"
