$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$base="$root/build/smoke/ChisBurner-dev.171-blank-save-r3"
$dst="$root/build/smoke/ChisBurner-dev.172-rtc-restore-r1"
$fpga="$root/build/rtc172-quartus"
$os="$root/.integration/openfpgaos/src/firmware/os/bld/rtc172"
function Sha($p){(Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash}
if((Test-Path $dst)-or(Test-Path "$dst.zip")){throw 'Preserve existing package'}
foreach($line in Get-Content "$base/SHA256SUMS.txt"){
 $hash,$p=$line -split '  ',2
 if((Sha "$base/SD_ROOT/$p") -ne $hash){throw "Base mismatch $p"}
}
if((Sha "$fpga/firmware.mif") -ne (Sha "$os/firmware.mif")){throw 'MIF mismatch'}
foreach($f in @('mif-update-172.log','asm-172.log')){
 if((Get-Content "$fpga/$f" -Raw) -notmatch 'successful'){throw "Unverified $f"}
}
& "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $fpga -AllowUnusedBridgeReady
. "$PSScriptRoot/os-image-integrity.ps1"
$null=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes("$os/os.bin"))
$app="$root/build/rtc172-app/app.elf"
$strings=[Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($app))
foreach($s in @('dev.172-rtc','Restore SAV + original RTC','Restore SAV + elapsed RTC','SAV and RTC restored / verified')){
 if(!$strings.Contains($s)){throw "Missing app string $s"}
}
$regression=Get-Content "$root/build/rtc172-integration.log" -Raw
foreach($s in @('RTC172 APP PASS','RTC172 OS PASS','RTC172 protocol PASS','Flash170 OS PASS','SAVE171 app PASS')){
 if(!$regression.Contains($s)){throw "Missing regression $s"}
}
$null=New-Item -ItemType Directory -Path $dst
Copy-Item "$base/SD_ROOT" "$dst/SD_ROOT" -Recurse
Copy-Item $app "$dst/SD_ROOT/Assets/chisburner/common/cb102.elf"
Copy-Item "$os/os.bin" "$dst/SD_ROOT/Assets/chisburner/common/os.bin"
Add-Type -TypeDefinition 'public static class Rtc172Reverse {public static byte[] Run(byte[] a){for(int i=0;i<a.Length;i++){int x=a[i],y=0;for(int k=0;k<8;k++){y=(y<<1)|(x&1);x>>=1;}a[i]=(byte)y;}return a;}}'
[IO.File]::WriteAllBytes("$dst/SD_ROOT/Cores/ChisBurner.ChisBurner/chis.rbf_r",[Rtc172Reverse]::Run([IO.File]::ReadAllBytes("$fpga/output_files/ap_core.rbf")))
$metaPath="$dst/SD_ROOT/Cores/ChisBurner.ChisBurner/core.json"
$meta=Get-Content $metaPath -Raw|ConvertFrom-Json
$meta.core.metadata.version='0.1.0-dev.172-rtc-restore-r1'
$utf8=[Text.UTF8Encoding]::new($false)
[IO.File]::WriteAllText($metaPath,($meta|ConvertTo-Json -Depth 20),$utf8)
Copy-Item "$root/docs/RTC172_TEST_CN.md" "$dst/TEST_CN.md"
Copy-Item "$root/docs/RTC172_DEVELOPMENT.md" "$dst/DEVELOPMENT.md"
foreach($f in @('rtc172-app-build.log','rtc172-os-build.log','rtc172-integration.log')){Copy-Item "$root/build/$f" "$dst/$f"}
Copy-Item "$base/ThirdParty" "$dst/ThirdParty" -Recurse
$null=New-Item -ItemType Directory -Path "$dst/Source/app","$dst/Source/OS","$dst/Source/tests","$dst/Source/core"
foreach($d in @('ui98','ui97','gba_db55')){Copy-Item "$root/app/$d" "$dst/Source/app/$d" -Recurse}
foreach($d in @('save','rtc')){Copy-Item "$root/core/$d" "$dst/Source/core/$d" -Recurse}
Copy-Item "$root/.integration/openfpgaos/src/firmware/os/kernel" "$dst/Source/OS/kernel" -Recurse
Copy-Item "$root/.integration/openfpgaos/src/firmware/os/targets/pocket/file.c" "$dst/Source/OS/pocket-file.c"
Copy-Item "$root/.integration/openfpgaos/src/firmware/api" "$dst/Source/OS/api" -Recurse
Copy-Item "$os/firmware.mif" "$dst/Source/OS/firmware.mif"
Get-ChildItem "$root/tests" -File | Where-Object Name -Match 'rtc17[2]|rtc167|save171' | Copy-Item -Destination "$dst/Source/tests"
foreach($f in @('mif-update-172.log','asm-172.log','sta-172.log','unconstrained.txt')){Copy-Item "$fpga/$f" "$dst/$f"}
$prefix=[IO.Path]::GetFullPath("$dst/SD_ROOT")+'\'
$sums=@(Get-ChildItem "$dst/SD_ROOT" -Recurse -File|Sort-Object FullName|ForEach-Object{"$(Sha $_.FullName)  $($_.FullName.Substring($prefix.Length).Replace('\','/'))"})
[IO.File]::WriteAllLines("$dst/SHA256SUMS.txt",[string[]]$sums,$utf8)
Compress-Archive -Path "$dst/*" -DestinationPath "$dst.zip"
Write-Host "PASS $dst.zip"
Get-FileHash "$dst.zip"
