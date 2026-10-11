$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$base="$root/build/smoke/ChisBurner-dev.174-cn-r5"
$dst="$root/build/smoke/ChisBurner-dev.178-mbc2"
$fit="$root/build/gb-save178-quartus"
$os="$root/.integration/openfpgaos/src/firmware/os/bld/gb-save178"
$app="$root/build/gb-save178-app/app.elf"
if((Test-Path -LiteralPath $dst)-or(Test-Path -LiteralPath "$dst.zip")){throw 'Preserve existing package'}
& "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $fit -AllowUnusedBridgeReady
if((Get-Content "$fit/output_files/ap_core.fit.summary" -Raw)-notmatch 'Fitter Status : Successful'){throw 'Fitter not successful'}
if((Get-FileHash "$os/firmware.mif").Hash-ne(Get-FileHash "$fit/firmware.mif").Hash){throw 'MIF mismatch'}
. "$PSScriptRoot/os-image-integrity.ps1"
$null=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes("$os/os.bin"))
if(![Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($app)).Contains('dev.178-mbc2')){throw 'Wrong app'}
foreach($line in Get-Content -LiteralPath "$base/SHA256SUMS.txt"){
 $hash,$path=$line -split '  ',2
 if((Get-FileHash -LiteralPath "$base/SD_ROOT/$path").Hash-ne$hash){throw "Base mismatch $path"}
}
New-Item -ItemType Directory -Path $dst | Out-Null
Copy-Item -LiteralPath "$base/SD_ROOT" -Destination "$dst/SD_ROOT" -Recurse
Copy-Item -LiteralPath $app -Destination "$dst/SD_ROOT/Assets/chisburner/common/cb102.elf"
Copy-Item -LiteralPath "$os/os.bin" -Destination "$dst/SD_ROOT/Assets/chisburner/common/os.bin"
# Pocket loads bit-reversed RBF, not the raw Quartus byte order.
Add-Type -TypeDefinition 'public static class Gb178Reverse {public static byte[] Run(byte[] a){for(int i=0;i<a.Length;i++){int x=a[i],y=0;for(int k=0;k<8;k++){y=(y<<1)|(x&1);x>>=1;}a[i]=(byte)y;}return a;}}'
[IO.File]::WriteAllBytes("$dst/SD_ROOT/Cores/ChisBurner.ChisBurner/chis.rbf_r",[Gb178Reverse]::Run([IO.File]::ReadAllBytes("$fit/output_files/ap_core.rbf")))
Copy-Item -LiteralPath "$root/docs/GB_SAVE178_TEST_CN.md" -Destination "$dst/README_CN.md"
$lines=Get-ChildItem -LiteralPath "$dst/SD_ROOT" -File -Recurse | Sort-Object FullName | ForEach-Object {
 $relative=$_.FullName.Substring(("$dst/SD_ROOT").Length+1).Replace('\','/')
 $hash=(Get-FileHash -LiteralPath $_.FullName).Hash
 if($relative-notin @('Assets/chisburner/common/cb102.elf','Assets/chisburner/common/os.bin','Cores/ChisBurner.ChisBurner/chis.rbf_r') -and $hash-ne(Get-FileHash -LiteralPath "$base/SD_ROOT/$relative").Hash){throw "Unexpected change: $relative"}
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
