param()
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$base="$root/build/smoke/ChisBurner-dev.149-auto/SD_ROOT/Cores/ChisBurner.ChisBurner/data.json"
$out="$root/build/saves153-selector"
if(Test-Path -LiteralPath $out){throw 'Output already exists; preserve previous experiment'}
$cfg=Get-Content -LiteralPath $base -Raw|ConvertFrom-Json
foreach($id in @(4,6)){
 $slot=@($cfg.data.data_slots|Where-Object {$_.id -eq $id})
 if($slot.Count -ne 1){throw "Expected one slot $id"}
 $slot=$slot[0]
 # Bit 0: native user reload. Bit 3: read-only. Bit 1 clear: common.
 # Nonvolatile selects Saves; deferload avoids automatic memory loading.
 $slot.parameters=9
 $slot.required=$false
 $slot.deferload=$true
 $slot|Add-Member -NotePropertyName nonvolatile -NotePropertyValue $true -Force
 if($id -eq 4){$slot.name='Saves ROM';$slot.extensions=@('gba','gb','gbc')}
 else{$slot.name='Saves SAV test';$slot.extensions=@('sav','bin')}
}
foreach($slot in $cfg.data.data_slots){
 if($slot.id -in @(4,6)){
  if(($slot.parameters -band 8) -eq 0 -or !$slot.deferload){throw 'Read-only/deferload guard failed'}
 }
}
New-Item -ItemType Directory -Path $out|Out-Null
Copy-Item -LiteralPath $base -Destination "$out/data.original.json"
$cfg|ConvertTo-Json -Depth 20|Set-Content -LiteralPath "$out/data.json" -Encoding ASCII
$roundtrip=Get-Content -LiteralPath "$out/data.json" -Raw|ConvertFrom-Json
foreach($slot in $roundtrip.data.data_slots){
 if($slot.id -notin @(4,6)){
  $old=(Get-Content -LiteralPath $base -Raw|ConvertFrom-Json).data.data_slots|Where-Object {$_.id -eq $slot.id}
  if(($old|ConvertTo-Json -Depth 20 -Compress) -ne ($slot|ConvertTo-Json -Depth 20 -Compress)){throw "Unrelated slot changed: $($slot.id)"}
 }
}
Write-Output "PASS: generated $out; only slots 4 and 6 changed; NOT installed"
