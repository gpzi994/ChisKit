param([string]$Version='dev.181-gb-empty',[string]$AppDir='gb-empty181-app',[string]$DocPrefix='GB_EMPTY181')
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$base="$root/build/smoke/ChisBurner-dev.180-r2"
$dst="$root/build/smoke/ChisBurner-$Version"
$app="$root/build/$AppDir/app.elf"
if((Test-Path -LiteralPath $dst)-or(Test-Path -LiteralPath "$dst.zip")){throw 'Preserve existing package'}
if(![Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($app)).Contains($Version)){throw 'Wrong app'}
foreach($line in Get-Content -LiteralPath "$base/SHA256SUMS.txt"){
 $hash,$path=$line -split '  ',2
 if((Get-FileHash -LiteralPath "$base/SD_ROOT/$path").Hash-ne$hash){throw "Base mismatch $path"}
}
New-Item -ItemType Directory -Path $dst | Out-Null
Copy-Item -LiteralPath "$base/SD_ROOT" -Destination "$dst/SD_ROOT" -Recurse
Copy-Item -LiteralPath $app -Destination "$dst/SD_ROOT/Assets/chisburner/common/cb102.elf"
Copy-Item -LiteralPath "$root/docs/${DocPrefix}_TEST_CN.md" -Destination "$dst/README_CN.md"
Copy-Item -LiteralPath "$root/docs/${DocPrefix}_BUILD_REPORT.md" -Destination "$dst/BUILD_REPORT.md"
$lines=Get-ChildItem -LiteralPath "$dst/SD_ROOT" -File -Recurse | Sort-Object FullName | ForEach-Object {
 $relative=$_.FullName.Substring(("$dst/SD_ROOT").Length+1).Replace('\','/')
 $hash=(Get-FileHash -LiteralPath $_.FullName).Hash
 if($relative-ne'Assets/chisburner/common/cb102.elf' -and $hash-ne(Get-FileHash -LiteralPath "$base/SD_ROOT/$relative").Hash){throw "Unexpected change: $relative"}
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
