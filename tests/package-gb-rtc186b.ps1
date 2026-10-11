$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$base="$root/build/smoke/ChisBurner-dev.186-gbrtc"
$dst="$root/build/smoke/ChisBurner-dev.186b-gbrtc"
$app="$root/build/gb-rtc186-app/app.elf"
if(Test-Path -LiteralPath $dst){throw 'Preserve existing package'}
foreach($line in Get-Content -LiteralPath "$base/SHA256SUMS.txt"){
 $hash,$p=$line -split '  ',2
 if((Get-FileHash -LiteralPath "$base/SD_ROOT/$p").Hash-ne$hash){throw "Base mismatch $p"}
}
if(![Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($app)).Contains('dev.186b-gbrtc')){throw 'Wrong app'}
New-Item -ItemType Directory -Path $dst | Out-Null
Copy-Item -LiteralPath "$base/SD_ROOT" -Destination "$dst/SD_ROOT" -Recurse
Copy-Item -LiteralPath $app -Destination "$dst/SD_ROOT/Assets/chisburner/common/cb102.elf"
Copy-Item -LiteralPath "$root/docs/GB_RTC186B_TEST_CN.md" -Destination "$dst/README_CN.md"
$lines=Get-ChildItem -LiteralPath "$dst/SD_ROOT" -File -Recurse | Sort-Object FullName | ForEach-Object {
 $p=$_.FullName.Substring(("$dst/SD_ROOT").Length+1).Replace('\','/')
 $hash=(Get-FileHash -LiteralPath $_.FullName).Hash
 if($p-ne'Assets/chisburner/common/cb102.elf' -and $hash-ne(Get-FileHash -LiteralPath "$base/SD_ROOT/$p").Hash){throw "Unexpected change $p"}
 '{0}  {1}' -f $hash,$p
}
[IO.File]::WriteAllLines("$dst/SHA256SUMS.txt",[string[]]$lines)
Compress-Archive -Path "$dst/*" -DestinationPath "$dst.zip"
$usb='D:/Assets/chisburner/common/cb102.elf'
foreach($p in @('Assets/chisburner/common/cb102.elf','Assets/chisburner/common/os.bin','Cores/ChisBurner.ChisBurner/chis.rbf_r')){
 if((Get-FileHash -LiteralPath "D:/$p").Hash-ne(Get-FileHash -LiteralPath "$base/SD_ROOT/$p").Hash){throw "USB differs from dev186: $p"}
}
$backup="$root/build/usb-before186b-$(Get-Date -Format yyyyMMdd-HHmmss)"
New-Item -ItemType Directory -Path $backup | Out-Null
Copy-Item -LiteralPath $usb -Destination "$backup/cb102.elf"
if((Get-FileHash "$backup/cb102.elf").Hash-ne(Get-FileHash $usb).Hash){throw 'Backup mismatch'}
Copy-Item -LiteralPath $app -Destination $usb -Force
if((Get-FileHash $usb).Hash-ne(Get-FileHash $app).Hash){throw 'USB readback mismatch'}
Write-Host "PASS: app-only update, rollback $backup; OS/FPGA/Saves untouched"
Get-FileHash -LiteralPath "$dst.zip"
