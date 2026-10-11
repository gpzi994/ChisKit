$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$base="$root/build/smoke/ChisBurner-dev.160-sram-r6"
$dst="$root/build/smoke/ChisBurner-dev.162-eeprom-r1"
$fpga="$root/build/eeprom162-seed3-quartus"
$os="$root/.integration/openfpgaos/src/firmware/os/bld/eeprom162"
$app="$root/build/save162-app/app.elf"
function Sha([string]$p){(Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash.ToUpperInvariant()}
if((Test-Path -LiteralPath $dst)-or(Test-Path -LiteralPath "$dst.zip")){throw 'Preserve existing package'}
foreach($line in Get-Content "$base/SHA256SUMS.txt"){
 $hash,$p=$line -split '  ',2
 if((Sha "$base/SD_ROOT/$p") -cne $hash.ToUpperInvariant()){throw "Base mismatch: $p"}
}
if((Get-Content "$fpga/full-compile.log" -Raw) -notmatch 'Full Compilation was successful'){
 throw 'FPGA compilation not complete'
}
if((Sha "$fpga/firmware.mif") -cne (Sha "$os/firmware.mif")){throw 'OS/FPGA MIF mismatch'}
& "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $fpga -AllowUnusedBridgeReady
. "$PSScriptRoot/os-image-integrity.ps1"
$null=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes("$os/os.bin"))
$strings=[Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($app))
foreach($phrase in @('dev.162-ee','Write EEPROM SAV','EEPROM write and full readback PASS')){
 if(!$strings.Contains($phrase)){throw "App missing $phrase"}
}
if(!(Test-Path -LiteralPath "$fpga/output_files/ap_core.rbf")){throw 'FPGA bitstream missing'}
$null=New-Item -ItemType Directory -Path $dst
Copy-Item -LiteralPath "$base/SD_ROOT" -Destination "$dst/SD_ROOT" -Recurse
$core="$dst/SD_ROOT/Cores/ChisBurner.ChisBurner"
$assets="$dst/SD_ROOT/Assets/chisburner/common"
Add-Type -TypeDefinition @'
public static class Eeprom162Bits { public static byte[] Reverse(byte[] a) { for(int i=0;i<a.Length;i++){int x=a[i],y=0;for(int b=0;b<8;b++){y=(y<<1)|(x&1);x>>=1;}a[i]=(byte)y;}return a;} }
'@
[IO.File]::WriteAllBytes("$core/chis.rbf_r",[Eeprom162Bits]::Reverse([IO.File]::ReadAllBytes("$fpga/output_files/ap_core.rbf")))
Copy-Item -LiteralPath "$os/os.bin" -Destination "$assets/os.bin" -Force
Copy-Item -LiteralPath $app -Destination "$assets/cb102.elf" -Force
$utf8=[Text.UTF8Encoding]::new($false)
$meta=Get-Content "$core/core.json" -Raw|ConvertFrom-Json
$meta.core.metadata.version='0.1.0-dev.162-eeprom-r1'
[IO.File]::WriteAllText("$core/core.json",($meta|ConvertTo-Json -Depth 20),$utf8)
foreach($name in @('ap_core.fit.summary','ap_core.sta.summary')){
 Copy-Item "$fpga/output_files/$name" "$dst/$name"
}
Copy-Item "$fpga/unconstrained.txt" "$dst/unconstrained.txt"
$prefix=[IO.Path]::GetFullPath("$dst/SD_ROOT")+'\'
$sums=@(Get-ChildItem "$dst/SD_ROOT" -Recurse -File|Sort-Object FullName|ForEach-Object{
 "$(Sha $_.FullName)  $($_.FullName.Substring($prefix.Length).Replace('\','/'))"
})
[IO.File]::WriteAllLines("$dst/SHA256SUMS.txt",[string[]]$sums,$utf8)
Compress-Archive -Path "$dst/*" -DestinationPath "$dst.zip"
Add-Type -AssemblyName System.IO.Compression.FileSystem
$zip=[IO.Compression.ZipFile]::OpenRead("$dst.zip")
try{foreach($line in $sums){
 $hash,$p=$line -split '  ',2
 $entry=$zip.GetEntry("SD_ROOT/$p")
 if(!$entry){$entry=$zip.GetEntry("SD_ROOT\$($p.Replace('/','\'))")}
 if(!$entry){throw "ZIP missing: $p"}
 $stream=$entry.Open();$sha=[Security.Cryptography.SHA256]::Create()
 try{$actual=([BitConverter]::ToString($sha.ComputeHash($stream))).Replace('-','');if($actual -cne $hash){throw "ZIP mismatch: $p"}}
 finally{$stream.Dispose();$sha.Dispose()}
}}finally{$zip.Dispose()}
Write-Host "PASS package: $dst.zip"
Get-FileHash -LiteralPath "$dst.zip" -Algorithm SHA256
