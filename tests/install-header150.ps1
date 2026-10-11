$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$base="$root/build/smoke/ChisBurner-dev.149-auto/SD_ROOT"
$app="$root/build/header150-app/app.elf"
if((Get-Content D:/Analogue_Pocket.json -Raw|ConvertFrom-Json).product -ne 'Analogue Pocket'){throw 'Wrong USB device'}
foreach($p in @('Assets/chisburner/common/os.bin','Cores/ChisBurner.ChisBurner/chis.rbf_r','Assets/chisburner/common/cb102.elf')){
 if((Get-FileHash "D:/$p").Hash -ne (Get-FileHash "$base/$p").Hash){throw "USB base mismatch $p"}
}
if(![Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($app)).Contains('dev.150-head')){throw 'Wrong app version'}
$backup="$root/build/usb-backup-header150"
if(Test-Path $backup){throw 'Preserve existing backup'}
New-Item -ItemType Directory $backup|Out-Null
Copy-Item D:/Assets/chisburner/common/cb102.elf "$backup/cb102.elf"
if((Get-FileHash "$backup/cb102.elf").Hash -ne (Get-FileHash D:/Assets/chisburner/common/cb102.elf).Hash){throw 'Backup mismatch'}
Copy-Item $app D:/Assets/chisburner/common/cb102.elf -Force
if((Get-FileHash $app).Hash -ne (Get-FileHash D:/Assets/chisburner/common/cb102.elf).Hash){throw 'Install mismatch'}
'PASS header150 app installed and SHA256 verified; OS/FPGA unchanged'
