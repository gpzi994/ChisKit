param([ValidateSet('r3','r5','r6')][string]$Revision='r3')
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$fpga="$root/build/eeprom139-diag-r3-quartus"
$os="$root/.integration/openfpgaos/src/firmware/os/bld/eeprom139-integration"
$app="$root/build/eeprom139-diag-r3-app/app.elf"
$dst="$root/build/smoke/ChisBurner-diag.139-ee-r3"
$version='ee139-r3'
$statusDoc='EEPROM139_R3_STATUS.md'
$testDoc='EEPROM139_R3_TEST_CN.md'
if($Revision -ne 'r3'){
 $fpga="$root/build/eeprom139-auto-$Revision-quartus"
 $app="$root/build/eeprom139-diag-$Revision-app/app.elf"
 $dst="$root/build/smoke/ChisBurner-diag.139-auto-$Revision"
 $version="ee139-$Revision"
 $statusDoc='SAVE139_R5_BUILD_STATUS.md'
 $testDoc='SAVE139_R5_TEST_CN.md'
 if($Revision -eq 'r6'){
  $statusDoc='SAVE139_R6_BUILD_STATUS.md'
  $testDoc='SAVE139_R6_TEST_CN.md'
  foreach($log in @('mif-update.log','assembler.log')){
   if((Get-Content "$fpga/$log" -Raw) -notmatch 'was successful'){throw "Failed $log"}
  }
 }
 $qsf=Get-Content "$fpga/ap_core.qsf" -Raw
 foreach($macro in @('CHISBURNER_SRAM137','CHISBURNER_SRAM138','CHISBURNER_SAVE139','CHISBURNER_EEPROM139')){
  if($qsf -notmatch "(?m)^set_global_assignment -name VERILOG_MACRO $macro\s*$"){throw "Missing macro: $macro"}
 }
}
function Sha($p){(Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash.ToLowerInvariant()}
if((Test-Path -LiteralPath $dst)-or(Test-Path -LiteralPath "$dst.zip")){throw 'Preserve prior package'}
if((Get-Content "$fpga/full-compile.log" -Raw) -notmatch 'Full Compilation was successful'){throw 'Compilation not complete'}
if((Sha "$os/firmware.mif") -cne (Sha "$fpga/firmware.mif")){throw 'MIF mismatch'}
& "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $fpga
. "$PSScriptRoot/os-image-integrity.ps1"
$null=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes("$os/os.bin"))
if(!([Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($app))).Contains($version)){throw 'Wrong application'}
$null=New-Item -ItemType Directory -Path $dst
Copy-Item -LiteralPath "$root/build/smoke/ChisBurner-smoke.135-timers/SD_ROOT" -Destination "$dst/SD_ROOT" -Recurse
$core="$dst/SD_ROOT/Cores/ChisBurner.ChisBurner"
$assets="$dst/SD_ROOT/Assets/chisburner/common"
Add-Type -TypeDefinition @'
public static class Eeprom139Bits { public static byte[] Reverse(byte[] a) { for(int i=0;i<a.Length;i++){int x=a[i],y=0;for(int b=0;b<8;b++){y=(y<<1)|(x&1);x>>=1;}a[i]=(byte)y;}return a;} }
'@
[IO.File]::WriteAllBytes("$core/chis.rbf_r",[Eeprom139Bits]::Reverse([IO.File]::ReadAllBytes("$fpga/output_files/ap_core.rbf")))
Copy-Item -LiteralPath "$os/os.bin" -Destination "$assets/os.bin" -Force
Copy-Item -LiteralPath $app -Destination "$assets/cb102.elf" -Force
$utf8=[Text.UTF8Encoding]::new($false)
$meta=Get-Content "$core/core.json" -Raw|ConvertFrom-Json
$meta.core.metadata.version=if($Revision -ne 'r3'){"0.1.0-diag.139-auto-$Revision"}else{'0.1.0-diag.139-ee-r3'}
[IO.File]::WriteAllText("$core/core.json",($meta|ConvertTo-Json -Depth 20),$utf8)
$data=Get-Content "$core/data.json" -Raw|ConvertFrom-Json
$slot=@($data.data.data_slots|Where-Object {$_.id -eq 5})
if($slot.Count -ne 1){throw 'Output slot missing'}
$slot[0].extensions=@('gba','gb','gbc','sav','bin')
[IO.File]::WriteAllText("$core/data.json",($data|ConvertTo-Json -Depth 20),$utf8)
$null=New-Item -ItemType Directory -Path "$dst/SD_ROOT/Saves/chisburner/common" -Force
Copy-Item -LiteralPath "$root/docs/$statusDoc" -Destination "$dst/README.md"
Copy-Item -LiteralPath "$root/docs/$testDoc" -Destination "$dst/TEST_CN.md"
if($Revision -eq 'r6'){
 Copy-Item -LiteralPath "$root/docs/SAVE139_ATMEL_SETTLE_FIX.md" -Destination "$dst/ATMEL_FIX.md"
 foreach($log in @('mif-update.log','assembler.log','scoped-sta.log')){
  Copy-Item -LiteralPath "$fpga/$log" -Destination "$dst/$log"
 }
}
if($Revision -ne 'r3'){
 Copy-Item -LiteralPath "$root/docs/SAVE139_AUTO_R5.md" -Destination "$dst/IMPLEMENTATION.md"
 foreach($report in @('unconstrained.txt','cdc-metastability.txt','cdc-net-delay-0.txt','cdc-net-delay-1.txt','cdc-net-delay-2.txt','cdc-net-delay-3.txt')){
  Copy-Item -LiteralPath "$fpga/$report" -Destination "$dst/$report"
 }
}
Copy-Item -LiteralPath "$PSScriptRoot/verify-eeprom139.ps1" -Destination "$dst/VERIFY_EEPROM.ps1"
foreach($report in @('ap_core.fit.summary','ap_core.sta.summary')){Copy-Item -LiteralPath "$fpga/output_files/$report" -Destination "$dst/$report"}
$prefix=[IO.Path]::GetFullPath("$dst/SD_ROOT")+'\'
$sums=@(Get-ChildItem "$dst/SD_ROOT" -Recurse -File|Sort-Object FullName|ForEach-Object{"$(Sha $_.FullName)  $($_.FullName.Substring($prefix.Length).Replace('\','/'))"})
[IO.File]::WriteAllLines("$dst/SHA256SUMS.txt",[string[]]$sums,$utf8)
Compress-Archive -Path "$dst/*" -DestinationPath "$dst.zip"
Add-Type -AssemblyName System.IO.Compression.FileSystem
$zip=[IO.Compression.ZipFile]::OpenRead("$dst.zip")
try{foreach($line in $sums){
 $hash,$file=$line -split '  ',2
 $entry=$zip.GetEntry("SD_ROOT/$file")
 if(!$entry){$entry=$zip.GetEntry("SD_ROOT\$($file.Replace('/','\'))")}
 if(!$entry){throw "ZIP missing $file"}
 $stream=$entry.Open();$sha=[Security.Cryptography.SHA256]::Create()
 try{$actual=([BitConverter]::ToString($sha.ComputeHash($stream))).Replace('-','').ToLowerInvariant()}finally{$stream.Dispose();$sha.Dispose()}
 if($actual -cne $hash){throw "ZIP hash mismatch $file"}
}}finally{$zip.Dispose()}
'PASS package ZIP hashes. Diagnostic only; external EEPROM timing unvalidated.'
Get-FileHash -LiteralPath "$dst.zip"
