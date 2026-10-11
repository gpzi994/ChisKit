$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$backup="$root/build/usb-backup-files156"
$app='D:/Assets/chisburner/common/cb102.elf'
$data='D:/Cores/ChisBurner.ChisBurner/data.json'
$new="$root/build/files156-app/app.elf"
if((Get-Content D:/Analogue_Pocket.json -Raw|ConvertFrom-Json).product -ne 'Analogue Pocket'){throw 'Wrong device'}
if((Get-FileHash $app).Hash -ne (Get-FileHash "$root/build/saves155-app/app.elf").Hash){throw 'App baseline mismatch'}
if((Get-FileHash $data).Hash -ne (Get-FileHash "$root/build/saves154-selector/data.json").Hash){throw 'Config baseline mismatch'}
if(![Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($new)).Contains('dev.156-files')){throw 'Wrong release'}
foreach($p in @('Assets/chisburner/common/os.bin','Cores/ChisBurner.ChisBurner/chis.rbf_r')){
 if((Get-FileHash "D:/$p").Hash -ne (Get-FileHash "$root/build/smoke/ChisBurner-dev.149-auto/SD_ROOT/$p").Hash){throw "Base mismatch $p"}
}
if(Test-Path -LiteralPath $backup){throw 'Preserve existing backup'}
New-Item -ItemType Directory -Path $backup|Out-Null
Copy-Item -LiteralPath $app -Destination "$backup/cb102.elf"
Copy-Item -LiteralPath $data -Destination "$backup/data.json"
if((Get-FileHash "$backup/cb102.elf").Hash -ne (Get-FileHash $app).Hash -or
   (Get-FileHash "$backup/data.json").Hash -ne (Get-FileHash $data).Hash){throw 'Backup mismatch'}
$cfg=Get-Content -LiteralPath $data -Raw|ConvertFrom-Json
($cfg.data.data_slots|Where-Object {$_.id -eq 4}).parameters=8
# Leave the proven new slot 20 intact; clear only the old slot's menu bit.
$cfg|ConvertTo-Json -Depth 20|Set-Content "$backup/data.next.json" -Encoding ASCII
Copy-Item -LiteralPath "$backup/data.next.json" -Destination $data -Force
Copy-Item -LiteralPath $new -Destination $app -Force
if((Get-FileHash $app).Hash -ne (Get-FileHash $new).Hash -or
   (Get-FileHash $data).Hash -ne (Get-FileHash "$backup/data.next.json").Hash){throw 'Install mismatch'}
'PASS dev.156-files installed and SHA256 verified; old ROM menu hidden; OS/FPGA unchanged'
