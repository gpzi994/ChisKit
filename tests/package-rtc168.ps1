$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$base="$root/build/smoke/ChisBurner-dev.167-rtc-read-r2"
$dst="$root/build/smoke/ChisBurner-dev.168-rtc-backup-r1"
function Sha($p){(Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash}
if((Test-Path $dst)-or(Test-Path "$dst.zip")){throw 'Preserve existing package'}
foreach($line in Get-Content "$base/SHA256SUMS.txt"){
 $hash,$p=$line -split '  ',2
 if((Sha "$base/SD_ROOT/$p") -ne $hash){throw "Base mismatch $p"}
}
$app="$root/build/rtc168-app/app.elf"
$strings=[Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($app))
foreach($s in @('dev.168-rtc-bak','Include RTC with this backup?','CHISBURNER_RTC_SIDECAR_V1','Backup Flash 128 KiB','Write EEPROM SAV')){
 if(!$strings.Contains($s)){throw "Missing app string $s"}
}
Copy-Item $base $dst -Recurse
Copy-Item $app "$dst/SD_ROOT/Assets/chisburner/common/cb102.elf" -Force
$metaPath="$dst/SD_ROOT/Cores/ChisBurner.ChisBurner/core.json"
$meta=Get-Content $metaPath -Raw|ConvertFrom-Json
$meta.core.metadata.version='0.1.0-dev.168-rtc-backup-r1'
$utf8=[Text.UTF8Encoding]::new($false)
[IO.File]::WriteAllText($metaPath,($meta|ConvertTo-Json -Depth 20),$utf8)
Copy-Item "$root/docs/RTC168_TEST_CN.md" "$dst/TEST_CN.md" -Force
Copy-Item "$root/build/rtc168-build.log" "$dst/RTC168_BUILD.log"
Copy-Item "$root/app/ui98/*" "$dst/Source/app/ui98/" -Recurse -Force
Copy-Item "$root/tests/test-rtc168.c","$root/tests/package-rtc168.ps1" "$dst/Source/tests/"
foreach($p in @('Assets/chisburner/common/os.bin','Cores/ChisBurner.ChisBurner/chis.rbf_r')){
 if((Sha "$dst/SD_ROOT/$p") -ne (Sha "$base/SD_ROOT/$p")){throw "OS/FPGA changed $p"}
}
$prefix=[IO.Path]::GetFullPath("$dst/SD_ROOT")+'\'
$sums=@(Get-ChildItem "$dst/SD_ROOT" -Recurse -File|Sort-Object FullName|ForEach-Object{"$(Sha $_.FullName)  $($_.FullName.Substring($prefix.Length).Replace('\','/'))"})
[IO.File]::WriteAllLines("$dst/SHA256SUMS.txt",[string[]]$sums,$utf8)
Compress-Archive -Path "$dst/*" -DestinationPath "$dst.zip"
Write-Host "PASS $dst.zip; OS and FPGA identical to verified 167-r2 pair"
Get-FileHash "$dst.zip"
