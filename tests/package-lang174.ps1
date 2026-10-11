$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$base=Join-Path $root 'build/smoke/ChisBurner-dev.173-rtc-edit-r3'
$dst=Join-Path $root 'build/smoke/ChisBurner-dev.174-cn-r5'
if((Test-Path -LiteralPath $dst)-or(Test-Path -LiteralPath "$dst.zip")){throw 'Preserve existing package'}
foreach($line in Get-Content -LiteralPath "$base/SHA256SUMS.txt"){
 $hash,$path=$line -split '  ',2
 if((Get-FileHash -LiteralPath "$base/SD_ROOT/$path").Hash -ne $hash){throw "Base mismatch: $path"}
}
$app=Join-Path $root 'build/lang174-app/app.elf'
if(![Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($app)).Contains('174-cn-r5')){throw 'Wrong app'}
New-Item -ItemType Directory -Path $dst | Out-Null
Copy-Item -LiteralPath "$base/SD_ROOT" -Destination "$dst/SD_ROOT" -Recurse
Copy-Item -LiteralPath $app -Destination "$dst/SD_ROOT/Assets/chisburner/common/cb102.elf"
# Bind the optional nonvolatile config slot so Pocket creates a datatable entry.
$manifestPath="$dst/SD_ROOT/Cores/ChisBurner.ChisBurner/data.json"
$manifest=Get-Content -Raw -LiteralPath $manifestPath | ConvertFrom-Json
$slot=$manifest.data.data_slots | Where-Object {$_.id -eq 8}
if(!$slot){throw 'Missing shared config slot'}
$slot | Add-Member -NotePropertyName filename -NotePropertyValue 'chisburner-ui.cfg' -Force
# Bit 2 clones slot 0's name and disables the NV slot under Play Cartridge.
# This is application configuration, independent of every cartridge/ROM.
$slot.parameters='0x80'
$manifest | ConvertTo-Json -Depth 30 | Set-Content -LiteralPath $manifestPath -Encoding utf8
Copy-Item -LiteralPath "$root/docs/LANG174_TEST_CN.md" -Destination "$dst/README_CN.md"
$lines=Get-ChildItem -LiteralPath "$dst/SD_ROOT" -File -Recurse | Sort-Object FullName | ForEach-Object {
 $relative=$_.FullName.Substring(("$dst/SD_ROOT").Length+1).Replace('\','/')
 $hash=(Get-FileHash -LiteralPath $_.FullName).Hash
 if($relative-notin @('Assets/chisburner/common/cb102.elf','Cores/ChisBurner.ChisBurner/data.json') -and $hash-ne(Get-FileHash -LiteralPath "$base/SD_ROOT/$relative").Hash){throw "Unexpected change: $relative"}
 '{0}  {1}' -f $hash,$relative
}
[IO.File]::WriteAllLines("$dst/SHA256SUMS.txt",[string[]]$lines)
Compress-Archive -Path "$dst/*" -DestinationPath "$dst.zip"
Add-Type -AssemblyName System.IO.Compression.FileSystem
$zip=[IO.Compression.ZipFile]::OpenRead("$dst.zip")
try{
 foreach($line in $lines){
  $hash,$path=$line -split '  ',2
  $entry=$zip.Entries | Where-Object {$_.FullName.Replace('\','/') -eq "SD_ROOT/$path"}
  if(!$entry){throw "ZIP missing $path"}
  $stream=$entry.Open();$sha=[Security.Cryptography.SHA256]::Create()
  try{$actual=[BitConverter]::ToString($sha.ComputeHash($stream)).Replace('-','')}finally{$stream.Dispose();$sha.Dispose()}
  if($actual-ne$hash){throw "ZIP mismatch $path"}
 }
}finally{$zip.Dispose()}
Get-FileHash -LiteralPath "$dst.zip"
