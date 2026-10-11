$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$target='D:/Cores/ChisBurner.ChisBurner/data.json'
$base="$root/build/saves153-selector/data.json"
$out="$root/build/saves154-selector"
if((Get-Content D:/Analogue_Pocket.json -Raw|ConvertFrom-Json).product -ne 'Analogue Pocket'){throw 'Wrong USB device'}
if((Get-FileHash $target).Hash -ne (Get-FileHash $base).Hash){throw 'Unexpected configuration baseline'}
if(Test-Path -LiteralPath $out){throw 'Preserve existing experiment and backup'}
$cfg=Get-Content -LiteralPath $base -Raw|ConvertFrom-Json
if(@($cfg.data.data_slots|Where-Object {$_.id -eq 20}).Count){throw 'Slot 20 already in use'}
$cfg.data.data_slots+= [pscustomobject][ordered]@{
 id=20; name='Saves ROM new'; required=$false; parameters=9
 extensions=@('gba','gb','gbc'); deferload=$true; nonvolatile=$true
}
if($cfg.data.data_slots.Count -gt 32){throw 'Too many slots'}
New-Item -ItemType Directory -Path $out|Out-Null
Copy-Item -LiteralPath $target -Destination "$out/data.before.json"
$cfg|ConvertTo-Json -Depth 20|Set-Content -LiteralPath "$out/data.json" -Encoding ASCII
$check=Get-Content -LiteralPath "$out/data.json" -Raw|ConvertFrom-Json
$original=Get-Content -LiteralPath $base -Raw|ConvertFrom-Json
foreach($slot in $original.data.data_slots){
 $same=@($check.data.data_slots|Where-Object {$_.id -eq $slot.id})
 if($same.Count -ne 1 -or ($same[0]|ConvertTo-Json -Depth 20 -Compress) -ne ($slot|ConvertTo-Json -Depth 20 -Compress)){throw "Existing slot changed: $($slot.id)"}
}
if((Get-FileHash "$out/data.before.json").Hash -ne (Get-FileHash $target).Hash){throw 'Backup mismatch'}
Copy-Item -LiteralPath "$out/data.json" -Destination $target -Force
if((Get-FileHash $target).Hash -ne (Get-FileHash "$out/data.json").Hash){throw 'Install mismatch'}
'PASS: added read-only deferred slot 20; all existing slots preserved; installed and SHA256 verified'
