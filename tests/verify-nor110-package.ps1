param([Parameter(Mandatory=$true)][string]$ZipPath,[int]$Revision=110)
$ErrorActionPreference='Stop'
Add-Type -AssemblyName System.IO.Compression.FileSystem
. (Join-Path $PSScriptRoot 'os-image-integrity.ps1')
$archive=[IO.Compression.ZipFile]::OpenRead([IO.Path]::GetFullPath($ZipPath))
try {
 $entries=@{}
 foreach($entry in $archive.Entries){
  $name=$entry.FullName.Replace('\','/')
  if($name.StartsWith('/') -or $name.Contains(':') -or $name -match '(^|/)\.\.(/|$)'){
   throw "Unsafe archive name: $name"
  }
  if($entries.ContainsKey($name)){throw "Duplicate archive name: $name"}
  $entries[$name]=$entry
 }
 function Read-ZipText([string]$name){
  if(-not $entries.ContainsKey($name)){throw "Missing archive entry: $name"}
  $reader=[IO.StreamReader]::new($entries[$name].Open())
  try {$reader.ReadToEnd()} finally {$reader.Dispose()}
 }
 function Check-Manifest([string]$name,[string]$prefix){
  $seen=@{};$count=0
  foreach($line in ((Read-ZipText $name) -split "`r?`n")){
   if(-not $line.Trim()){continue}
   if($line -notmatch '^([0-9a-f]{64})  (.+)$'){throw "Invalid manifest: $name"}
   $expected=$Matches[1];$relative=$Matches[2];$path=$prefix+$relative
   if($seen.ContainsKey($relative)){throw "Duplicate manifest entry: $path"}
   $seen[$relative]=$true
   if(-not $entries.ContainsKey($path)){throw "Missing payload: $path"}
   $stream=$entries[$path].Open();$sha=[Security.Cryptography.SHA256]::Create()
   try {$actual=[BitConverter]::ToString($sha.ComputeHash($stream)).Replace('-','').ToLowerInvariant()}
   finally {$sha.Dispose();$stream.Dispose()}
   if($actual -cne $expected){throw "Payload hash mismatch: $path"}
   $count++
  }
  foreach($path in $entries.Keys){
   if($path.StartsWith($prefix) -and -not $path.EndsWith('/')){
    if(-not $seen.ContainsKey($path.Substring($prefix.Length))){throw "Unlisted payload: $path"}
   }
  }
  if(-not $count){throw "Empty manifest: $name"}
  return $count
 }
 $sdCount=Check-Manifest 'SHA256SUMS.txt' 'SD_ROOT/'
 $sourceCount=Check-Manifest 'SOURCES_SHA256.txt' 'DEVELOPMENT_SOURCE/'
 $infoNames=@($entries.Keys | Where-Object {$_ -like 'SD_ROOT/Cores/*/info.txt'})
 if($infoNames.Count -ne 1){throw 'Expected one core info.txt'}
 if((Read-ZipText $infoNames[0]) -notmatch [regex]::Escape("smoke.$Revision NOR hardware TEST")){
  throw 'Core info version mismatch'
 }
 foreach($path in $entries.Keys){
  if($path -match '\.(gba|gbc|gb)$'){throw "Unexpected game ROM in package: $path"}
 }
 Write-Output "PASS ZIP manifests: $sdCount SD files; $sourceCount source files; version smoke.$Revision; no game ROM"
} finally {$archive.Dispose()}
$null=Assert-ChisBurnerZipOsIntegrity -ZipPath $ZipPath
Get-FileHash -LiteralPath $ZipPath -Algorithm SHA256
