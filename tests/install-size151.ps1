param([ValidateSet('151','152')][string]$Revision='151')
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$base="$root/build/smoke/ChisBurner-dev.149-auto/SD_ROOT"
$app="$root/build/size$Revision-app/app.elf"
if((Get-Content D:/Analogue_Pocket.json -Raw|ConvertFrom-Json).product -ne 'Analogue Pocket'){throw 'Wrong USB device'}
foreach($p in @('Assets/chisburner/common/os.bin','Cores/ChisBurner.ChisBurner/chis.rbf_r')){
 if((Get-FileHash "D:/$p").Hash -ne (Get-FileHash "$base/$p").Hash){throw "USB base mismatch $p"}
}
$previous=if($Revision -eq '152'){'size151-app'}else{'header150-app'}
if((Get-FileHash D:/Assets/chisburner/common/cb102.elf).Hash -ne (Get-FileHash "$root/build/$previous/app.elf").Hash){throw 'App base mismatch'}
if(![Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($app)).Contains("dev.$Revision-size")){throw 'Wrong app version'}
$backup="$root/build/usb-backup-size$Revision"
if(Test-Path $backup){throw 'Preserve existing backup'}
New-Item -ItemType Directory $backup|Out-Null
Copy-Item D:/Assets/chisburner/common/cb102.elf "$backup/cb102.elf"
if((Get-FileHash "$backup/cb102.elf").Hash -ne (Get-FileHash D:/Assets/chisburner/common/cb102.elf).Hash){throw 'Backup mismatch'}
Copy-Item $app D:/Assets/chisburner/common/cb102.elf -Force
if((Get-FileHash $app).Hash -ne (Get-FileHash D:/Assets/chisburner/common/cb102.elf).Hash){throw 'Install mismatch'}
"PASS size$Revision app installed and SHA256 verified; OS/FPGA unchanged"
