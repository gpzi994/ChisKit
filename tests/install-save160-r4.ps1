param([ValidateSet('r4','r5','r6')][string]$Revision='r4')
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$media='D:'
$package="$root/build/smoke/ChisBurner-dev.160-sram-$Revision/SD_ROOT"
$previous="$root/build/smoke/ChisBurner-dev.160-sram-r3/SD_ROOT"
$backup="$root/build/usb-backup-save160-$Revision"
if($Revision -eq 'r5'){$previous="$root/build/smoke/ChisBurner-dev.160-sram-r4/SD_ROOT"}
if($Revision -eq 'r6'){$previous="$root/build/smoke/ChisBurner-dev.160-sram-r5/SD_ROOT"}
function Sha([string]$p){(Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash}
if((Get-Content "$media/Analogue_Pocket.json" -Raw|ConvertFrom-Json).product -ne 'Analogue Pocket'){throw 'Wrong USB device'}
if(Test-Path -LiteralPath $backup){throw 'Preserve existing backup'}
$items=@('Cores/ChisBurner.ChisBurner/chis.rbf_r','Assets/chisburner/common/os.bin','Assets/chisburner/common/cb102.elf','Cores/ChisBurner.ChisBurner/core.json','Cores/ChisBurner.ChisBurner/data.json')
foreach($p in $items){
 if((Sha "$media/$p") -ne (Sha "$previous/$p")){throw "Unexpected installed file: $p"}
 if(!(Test-Path "$package/$p")){throw "Missing package file: $p"}
}
$null=New-Item -ItemType Directory -Path $backup
foreach($p in $items){
 $saved="$backup/$([IO.Path]::GetFileName($p))"
 Copy-Item -LiteralPath "$media/$p" -Destination $saved
 if((Sha $saved) -ne (Sha "$media/$p")){throw 'Backup mismatch'}
}
foreach($p in $items){
 Copy-Item -LiteralPath "$package/$p" -Destination "$media/$p" -Force
 if((Sha "$media/$p") -ne (Sha "$package/$p")){throw "Install mismatch: $p"}
}
Write-Host "PASS: dev.160-$Revision installed; previous five files backed up; Saves not modified."
