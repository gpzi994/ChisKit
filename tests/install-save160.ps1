$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$media='D:'
$package="$root/build/smoke/ChisBurner-dev.160-sram-r2/SD_ROOT"
$baseline="$root/build/smoke/ChisBurner-dev.149-auto/SD_ROOT"
$backup="$root/build/usb-backup-save160"
function Sha([string]$p){(Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash.ToUpperInvariant()}
if((Get-Content -LiteralPath "$media/Analogue_Pocket.json" -Raw|ConvertFrom-Json).product -ne 'Analogue Pocket'){throw 'Wrong USB device'}
if(Test-Path -LiteralPath $backup){throw 'Existing backup; do not overwrite'}
$items=@(
    [pscustomobject]@{name='FPGA';rel='Cores/ChisBurner.ChisBurner/chis.rbf_r';expected="$baseline/Cores/ChisBurner.ChisBurner/chis.rbf_r"},
    [pscustomobject]@{name='OS';rel='Assets/chisburner/common/os.bin';expected="$baseline/Assets/chisburner/common/os.bin"},
    [pscustomobject]@{name='Application';rel='Assets/chisburner/common/cb102.elf';expected="$root/build/sav157-app/app.elf"},
    [pscustomobject]@{name='Saves picker';rel='Cores/ChisBurner.ChisBurner/data.json';expected="$root/build/usb-backup-files156/data.next.json"},
    [pscustomobject]@{name='Core metadata';rel='Cores/ChisBurner.ChisBurner/core.json';expected="$baseline/Cores/ChisBurner.ChisBurner/core.json"}
)
foreach($i in $items){
    $live="$media/$($i.rel)";$next="$package/$($i.rel)"
    if(!(Test-Path -LiteralPath $live) -or !(Test-Path -LiteralPath $next)){throw "Missing $($i.name)"}
    if((Sha $live) -cne (Sha $i.expected)){throw "Unexpected current $($i.name); stop"}
}
$null=New-Item -ItemType Directory -Path $backup
foreach($i in $items){
    $saved=Join-Path $backup ([IO.Path]::GetFileName($i.rel))
    Copy-Item -LiteralPath "$media/$($i.rel)" -Destination $saved
    if((Sha $saved) -cne (Sha "$media/$($i.rel)")){throw "Backup mismatch $($i.name); install not started"}
}
foreach($i in $items){
    $live="$media/$($i.rel)";$next="$package/$($i.rel)"
    Copy-Item -LiteralPath $next -Destination $live -Force
    if((Sha $live) -cne (Sha $next)){throw "Install mismatch $($i.name); inspect backup"}
}
Write-Host "PASS installed dev.160-sram-r2; five current files backed up at $backup; Saves content untouched"
foreach($i in $items){Write-Host "$($i.name): $(Sha "$media/$($i.rel)" )"}
