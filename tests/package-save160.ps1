param([ValidateSet('r2','r3','r4','r5','r6')][string]$Revision='r4')
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$base="$root/build/smoke/ChisBurner-dev.149-auto"
$dst="$root/build/smoke/ChisBurner-dev.160-sram-$Revision"
$fpga="$root/build/save160-quartus"
$os="$root/.integration/openfpgaos/src/firmware/os/bld/save160-integration"
$app="$root/build/save160-app/app.elf"
$version='dev.160-sram'
if($Revision -eq 'r3'){$app="$root/build/save160-r3-app/app.elf";$version='dev.160-r3'}
if($Revision -in @('r4','r5')){
 $app="$root/build/save160-$Revision-app/app.elf";$version="dev.160-$Revision"
 $os="$root/.integration/openfpgaos/src/firmware/os/bld/save160-$Revision"
 $fpga="$root/build/save160-$Revision-quartus"
 if((Get-Content "$fpga/assembler.log" -Raw) -notmatch 'successful'){throw 'R4 assembler missing'}
}
if((Test-Path -LiteralPath $dst)-or(Test-Path -LiteralPath "$dst.zip")){throw 'Preserve existing package'}
if($Revision -eq 'r6'){
 $app="$root/build/save160-r6-app/app.elf";$version='dev.160-r6'
 $os="$root/.integration/openfpgaos/src/firmware/os/bld/save160-r5"
 $fpga="$root/build/save160-r5-quartus"
}
function Sha([string]$p){(Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash.ToUpperInvariant()}
foreach($line in Get-Content "$base/SHA256SUMS.txt"){
    $hash,$p=$line -split '  ',2
    if((Sha "$base/SD_ROOT/$p") -cne $hash.ToUpperInvariant()){throw "Base mismatch: $p"}
}
if((Get-Content "$fpga/full-compile.log" -Raw) -notmatch 'Full Compilation was successful'){throw 'FPGA compile missing'}
if((Sha "$fpga/firmware.mif") -cne (Sha "$os/firmware.mif")){throw 'OS/FPGA MIF mismatch'}
$qsf=Get-Content "$fpga/ap_core.qsf" -Raw
foreach($macro in @('CHISBURNER_SRAM137','CHISBURNER_SRAM138','CHISBURNER_SAVE139','CHISBURNER_EEPROM139','CHISBURNER_SAVE158')){
    if($qsf -notmatch "(?m)^set_global_assignment -name VERILOG_MACRO $macro\s*$"){throw "Missing FPGA macro: $macro"}
}
& "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $fpga
. "$PSScriptRoot/os-image-integrity.ps1"
$null=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes("$os/os.bin"))
$strings=[Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($app))
foreach($s in @($version,'Write SAV','Original SAV verified; confirm write')){
    if(!$strings.Contains($s)){throw "App missing: $s"}
}
if($Revision -in @('r3','r4','r5','r6') -and !$strings.Contains('Detecting save hardware...')){throw 'Automatic save detection missing'}
if($Revision -eq 'r6' -and !$strings.Contains('Backup EEPROM')){throw 'EEPROM backup missing'}
$null=New-Item -ItemType Directory -Path $dst
Copy-Item -LiteralPath "$base/SD_ROOT" -Destination "$dst/SD_ROOT" -Recurse
$core="$dst/SD_ROOT/Cores/ChisBurner.ChisBurner"
$assets="$dst/SD_ROOT/Assets/chisburner/common"
Add-Type -TypeDefinition @'
public static class Save160Bits { public static byte[] Reverse(byte[] a) { for(int i=0;i<a.Length;i++){int x=a[i],y=0;for(int b=0;b<8;b++){y=(y<<1)|(x&1);x>>=1;}a[i]=(byte)y;}return a;} }
'@
[IO.File]::WriteAllBytes("$core/chis.rbf_r",[Save160Bits]::Reverse([IO.File]::ReadAllBytes("$fpga/output_files/ap_core.rbf")))
Copy-Item -LiteralPath "$os/os.bin" -Destination "$assets/os.bin" -Force
Copy-Item -LiteralPath $app -Destination "$assets/cb102.elf" -Force
$utf8=[Text.UTF8Encoding]::new($false)
$meta=Get-Content "$core/core.json" -Raw|ConvertFrom-Json
$meta.core.metadata.version="0.1.0-dev.160-sram-$Revision"
[IO.File]::WriteAllText("$core/core.json",($meta|ConvertTo-Json -Depth 20),$utf8)
$cfg=Get-Content "$core/data.json" -Raw|ConvertFrom-Json
if(@($cfg.data.data_slots|Where-Object {$_.id -eq 20}).Count){throw 'Input slot 20 already present'}
$slot4=@($cfg.data.data_slots|Where-Object {$_.id -eq 4})
$slot6=@($cfg.data.data_slots|Where-Object {$_.id -eq 6})
if($slot4.Count -ne 1 -or $slot6.Count -ne 1){throw 'Expected ROM and SAV slots'}
$slot4[0].name='Saves ROM';$slot4[0].parameters=8;$slot4[0].required=$false;$slot4[0].deferload=$true
$slot4[0]|Add-Member -NotePropertyName nonvolatile -NotePropertyValue $true -Force
$slot6[0].name='Saves SAV test';$slot6[0].parameters=9;$slot6[0].required=$false;$slot6[0].deferload=$true
$slot6[0].extensions=@('sav','bin')
$slot6[0]|Add-Member -NotePropertyName nonvolatile -NotePropertyValue $true -Force
$cfg.data.data_slots+=[pscustomobject][ordered]@{
    id=20;name='Saves ROM new';required=$false;parameters=9
    extensions=@('gba','gb','gbc');deferload=$true;nonvolatile=$true
}
if($cfg.data.data_slots.Count -gt 32){throw 'Too many Pocket slots'}
[IO.File]::WriteAllText("$core/data.json",($cfg|ConvertTo-Json -Depth 20),$utf8)
Copy-Item -LiteralPath "$root/docs/SAVE160_TEST_CN.md" -Destination "$dst/README.md"
foreach($name in @('ap_core.fit.summary','ap_core.sta.summary')){Copy-Item "$fpga/output_files/$name" "$dst/$name"}
Copy-Item "$fpga/unconstrained.txt" "$dst/unconstrained.txt"
$prefix=[IO.Path]::GetFullPath("$dst/SD_ROOT")+'\'
$sums=@(Get-ChildItem "$dst/SD_ROOT" -Recurse -File|Sort-Object FullName|ForEach-Object{"$(Sha $_.FullName)  $($_.FullName.Substring($prefix.Length).Replace('\','/'))"})
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
