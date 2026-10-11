param([switch]$Stages141)
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$base="$root/build/smoke/ChisBurner-diag.139-auto-r6"
$dst="$root/build/smoke/ChisBurner-diag.140-rom"
if(!$Stages141 -and ((Test-Path $dst)-or(Test-Path "$dst.zip"))){throw 'Preserve old package'}
$app="$root/build/gba-header140-app/app.elf"
$version='diag.140-rom';$doc='GBA_HEADER140_TEST_CN.md'
if($Stages141){
 $dst="$root/build/smoke/ChisBurner-diag.141-stages"
 if((Test-Path $dst)-or(Test-Path "$dst.zip")){throw 'Preserve old stages package'}
 $app="$root/build/stages141-app/app.elf";$version='diag.141-stages';$doc='STAGES141_TEST_CN.md'
 $fpga="$root/build/stages141-quartus"
 & "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $fpga
 foreach($log in @('mif-update.log','assembler.log')){if((Get-Content "$fpga/$log" -Raw) -notmatch 'was successful'){throw "Failed $log"}}
 $os="$root/.integration/openfpgaos/src/firmware/os/bld/eeprom139-integration"
 if((Get-FileHash "$os/firmware.mif").Hash -ne (Get-FileHash "$fpga/firmware.mif").Hash){throw 'MIF mismatch'}
 . "$PSScriptRoot/os-image-integrity.ps1"
 $null=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes("$os/os.bin"))
}
if(!([Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($app))).Contains($version)){throw 'Wrong app'}
Copy-Item -LiteralPath "$base/SD_ROOT" -Destination "$dst/SD_ROOT" -Recurse
Copy-Item -LiteralPath $app -Destination "$dst/SD_ROOT/Assets/chisburner/common/cb102.elf" -Force
foreach($p in @('Cores/ChisBurner.ChisBurner/chis.rbf_r','Assets/chisburner/common/os.bin')){
 if((Get-FileHash "$base/SD_ROOT/$p").Hash -ne (Get-FileHash "$dst/SD_ROOT/$p").Hash){throw 'R6 pair mismatch'}
}
if($Stages141){
 Copy-Item -LiteralPath "$os/os.bin" -Destination "$dst/SD_ROOT/Assets/chisburner/common/os.bin" -Force
 Add-Type -TypeDefinition 'public static class StageBits { public static byte[] Rev(byte[] a){for(int i=0;i<a.Length;i++){int x=a[i],y=0;for(int b=0;b<8;b++){y=(y<<1)|(x&1);x>>=1;}a[i]=(byte)y;}return a;} }'
 [IO.File]::WriteAllBytes("$dst/SD_ROOT/Cores/ChisBurner.ChisBurner/chis.rbf_r",[StageBits]::Rev([IO.File]::ReadAllBytes("$fpga/output_files/ap_core.rbf")))
 foreach($log in @('mif-update.log','assembler.log','scoped-sta.log','unconstrained.txt')){Copy-Item "$fpga/$log" "$dst/$log"}
}
Copy-Item -LiteralPath "$root/docs/$doc" -Destination "$dst/README.md"
$prefix=[IO.Path]::GetFullPath("$dst/SD_ROOT")+'\'
$sums=@(Get-ChildItem "$dst/SD_ROOT" -Recurse -File|ForEach-Object{"$((Get-FileHash $_.FullName).Hash)  $($_.FullName.Substring($prefix.Length).Replace('\','/'))"})
[IO.File]::WriteAllLines("$dst/SHA256SUMS.txt",[string[]]$sums)
Compress-Archive -Path "$dst/*" -DestinationPath "$dst.zip"
Add-Type -AssemblyName System.IO.Compression.FileSystem
$z=[IO.Compression.ZipFile]::OpenRead("$dst.zip")
try{foreach($line in $sums){$hash,$p=$line -split '  ',2;$e=$z.GetEntry("SD_ROOT/$p");if(!$e){$e=$z.GetEntry("SD_ROOT\$($p.Replace('/','\'))")};if(!$e){throw "Missing $p"};$s=$e.Open();$h=[Security.Cryptography.SHA256]::Create();try{$actual=([BitConverter]::ToString($h.ComputeHash($s))).Replace('-','');if($actual -ne $hash){throw "Hash mismatch $p"}}finally{$s.Dispose();$h.Dispose()}}}finally{$z.Dispose()}
'PASS matched OS/FPGA and all ZIP payload hashes verified'
