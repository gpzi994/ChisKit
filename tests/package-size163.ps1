$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$base=Join-Path $root 'build/smoke/ChisBurner-dev.162-eeprom-r1'
$dst=Join-Path $root 'build/smoke/ChisBurner-dev.163-size-r2'
$app=Join-Path $root 'build/size163-app/app.elf'
function Sha([string]$path){(Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash.ToUpperInvariant()}
if((Test-Path -LiteralPath $dst)-or(Test-Path -LiteralPath "$dst.zip")){throw 'Preserve existing package'}
if(!(Test-Path -LiteralPath $app)){throw 'Build size163 app first'}
$strings=[Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($app))
foreach($phrase in @('dev.163-size','Write EEPROM SAV','EEPROM write and full readback PASS')){
 if(!$strings.Contains($phrase)){throw "App missing $phrase"}
}
foreach($line in Get-Content (Join-Path $base 'SHA256SUMS.txt')){
 $hash,$relative=$line -split '  ',2
 if((Sha (Join-Path "$base/SD_ROOT" $relative)) -cne $hash.ToUpperInvariant()){
  throw "Base mismatch: $relative"
 }
}
$null=New-Item -ItemType Directory -Path $dst
Copy-Item -LiteralPath "$base/SD_ROOT" -Destination "$dst/SD_ROOT" -Recurse
$core="$dst/SD_ROOT/Cores/ChisBurner.ChisBurner"
$assets="$dst/SD_ROOT/Assets/chisburner/common"
Copy-Item -LiteralPath $app -Destination "$assets/cb102.elf" -Force
$utf8=[Text.UTF8Encoding]::new($false)
$meta=Get-Content "$core/core.json" -Raw | ConvertFrom-Json
$meta.core.metadata.version='0.1.0-dev.163-size-r2'
[IO.File]::WriteAllText("$core/core.json",($meta|ConvertTo-Json -Depth 20),$utf8)
foreach($relative in @('Assets/chisburner/common/os.bin','Cores/ChisBurner.ChisBurner/chis.rbf_r')){
 if((Sha (Join-Path "$base/SD_ROOT" $relative)) -cne (Sha (Join-Path "$dst/SD_ROOT" $relative))){
  throw "Unchanged firmware differs: $relative"
 }
}
$prefix=[IO.Path]::GetFullPath("$dst/SD_ROOT")+'\'
$sums=@(Get-ChildItem "$dst/SD_ROOT" -Recurse -File | Sort-Object FullName | ForEach-Object{
 "$(Sha $_.FullName)  $($_.FullName.Substring($prefix.Length).Replace('\','/'))"
})
[IO.File]::WriteAllLines("$dst/SHA256SUMS.txt",[string[]]$sums,$utf8)
Compress-Archive -Path "$dst/*" -DestinationPath "$dst.zip"
Add-Type -AssemblyName System.IO.Compression.FileSystem
$zip=[IO.Compression.ZipFile]::OpenRead("$dst.zip")
try{
 foreach($line in $sums){
  $hash,$relative=$line -split '  ',2
  $entry=$zip.GetEntry("SD_ROOT/$relative")
  if(!$entry){$entry=$zip.GetEntry("SD_ROOT\$($relative.Replace('/','\'))")}
  if(!$entry){throw "ZIP missing: $relative"}
  $stream=$entry.Open();$sha=[Security.Cryptography.SHA256]::Create()
  try{
   $actual=([BitConverter]::ToString($sha.ComputeHash($stream))).Replace('-','')
   if($actual -cne $hash){throw "ZIP mismatch: $relative"}
  }finally{$stream.Dispose();$sha.Dispose()}
 }
}finally{$zip.Dispose()}
Write-Host "PASS package: $dst.zip"
Get-FileHash -LiteralPath "$dst.zip" -Algorithm SHA256
