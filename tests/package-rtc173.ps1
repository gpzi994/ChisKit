$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$base=Join-Path $root 'build/smoke/ChisBurner-dev.172-rtc-restore-r1'
$dst=Join-Path $root 'build/smoke/ChisBurner-dev.173-rtc-edit-r3'
if((Test-Path -LiteralPath $dst)-or(Test-Path -LiteralPath "$dst.zip")){throw 'Preserve existing package'}
foreach($line in Get-Content -LiteralPath "$base/SHA256SUMS.txt"){
 $hash,$path=$line -split '  ',2
 if((Get-FileHash -LiteralPath "$base/SD_ROOT/$path").Hash -ne $hash){throw "Base mismatch: $path"}
}
$app=Join-Path $root 'build/rtc173-app/app.elf'
if(![Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($app)).Contains('dev.173-rtc-r3')){throw 'Wrong app'}
New-Item -ItemType Directory -Path $dst | Out-Null
Copy-Item -LiteralPath "$base/SD_ROOT" -Destination "$dst/SD_ROOT" -Recurse
Copy-Item -LiteralPath $app -Destination "$dst/SD_ROOT/Assets/chisburner/common/cb102.elf"
Copy-Item -LiteralPath "$root/docs/RTC173_TEST_CN.md" -Destination "$dst/README_CN.md"
# Retain core metadata: FPGA/OS are exactly the validated dev.172 pair.
$lines=Get-ChildItem -LiteralPath "$dst/SD_ROOT" -File -Recurse | Sort-Object FullName | ForEach-Object {
 $relative=$_.FullName.Substring(("$dst/SD_ROOT").Length+1).Replace('\','/')
 '{0}  {1}' -f (Get-FileHash -LiteralPath $_.FullName).Hash,$relative
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
