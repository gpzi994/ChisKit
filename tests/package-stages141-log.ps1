param([switch]$InstallUsb,[ValidateSet('log','log2','log3','read142','read143','read144','read145','wait146','auto149')][string]$Revision='log3')
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$base="$root/build/smoke/ChisBurner-diag.141-stages"
$dst="$root/build/smoke/ChisBurner-diag.141-$Revision"
$app="$root/build/stages141-$Revision-app/app.elf"
$version="diag.141-$Revision"
if($Revision -eq 'read142'){$version='diag.142-read';$dst="$root/build/smoke/ChisBurner-$version"}
if($Revision -eq 'read143'){$version='diag.143-read';$dst="$root/build/smoke/ChisBurner-$version"}
if($Revision -eq 'read144'){$version='diag.144-read';$dst="$root/build/smoke/ChisBurner-$version"}
if($Revision -eq 'read145'){$version='diag.145-bank';$dst="$root/build/smoke/ChisBurner-$version"}
if($Revision -eq 'wait146'){$version='diag.146-wait';$dst="$root/build/smoke/ChisBurner-$version"}
if($Revision -eq 'auto149'){$version='dev.149-auto';$dst="$root/build/smoke/ChisBurner-$version"}
if((Test-Path $dst)-or(Test-Path "$dst.zip")){throw 'Preserve existing release'}
if(!([Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($app))).Contains($version)){throw 'Wrong app version'}
# Validate the immutable matched release before reusing its OS/FPGA.
foreach($line in Get-Content "$base/SHA256SUMS.txt"){
 $hash,$p=$line -split '  ',2
 if((Get-FileHash -LiteralPath "$base/SD_ROOT/$p").Hash -ne $hash){throw "Base mismatch $p"}
}
Copy-Item -LiteralPath "$base/SD_ROOT" -Destination "$dst/SD_ROOT" -Recurse
Copy-Item -LiteralPath $app -Destination "$dst/SD_ROOT/Assets/chisburner/common/cb102.elf"
Copy-Item -LiteralPath "$root/docs/STAGES141_LOG_CN.md" -Destination "$dst/README.md"
if($Revision -eq 'read142'){Copy-Item -LiteralPath "$root/docs/SAVE_READ142_CN.md" -Destination "$dst/README.md" -Force}
if($Revision -in @('read143','read144','read145','wait146','auto149')){
 $fpga="$root/build/$Revision-quartus"
 & "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $fpga
 foreach($name in @('mif-update.log','assembler.log')){if((Get-Content "$fpga/$name" -Raw) -notmatch 'was successful'){throw "Failed $name"}}
 $os="$root/.integration/openfpgaos/src/firmware/os/bld/eeprom139-integration"
 if((Get-FileHash "$os/firmware.mif").Hash -ne (Get-FileHash "$fpga/firmware.mif").Hash){throw 'MIF mismatch'}
 . "$PSScriptRoot/os-image-integrity.ps1"
 $null=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes("$os/os.bin"))
 Copy-Item "$os/os.bin" "$dst/SD_ROOT/Assets/chisburner/common/os.bin" -Force
 Add-Type -TypeDefinition 'public static class Read143Bits {public static byte[] Rev(byte[] a){for(int i=0;i<a.Length;i++){int x=a[i],y=0;for(int b=0;b<8;b++){y=(y<<1)|(x&1);x>>=1;}a[i]=(byte)y;}return a;}}'
 [IO.File]::WriteAllBytes("$dst/SD_ROOT/Cores/ChisBurner.ChisBurner/chis.rbf_r",[Read143Bits]::Rev([IO.File]::ReadAllBytes("$fpga/output_files/ap_core.rbf")))
 Copy-Item "$root/docs/SAVE_READ143_CN.md" "$dst/README.md" -Force
 if($Revision -eq 'read144'){Copy-Item "$root/docs/SAVE_READ144_CN.md" "$dst/README.md" -Force}
}
if($Revision -eq 'read145'){Copy-Item "$root/docs/SAVE_READ145_CN.md" "$dst/README.md" -Force}
if($Revision -eq 'wait146'){Copy-Item "$root/docs/SAVE_WAIT146_CN.md" "$dst/README.md" -Force}
if($Revision -eq 'auto149'){Copy-Item "$root/docs/SAVE149_CN.md" "$dst/README.md" -Force}
$prefix=[IO.Path]::GetFullPath("$dst/SD_ROOT")+'\'
$sums=@(Get-ChildItem "$dst/SD_ROOT" -Recurse -File|ForEach-Object{"$((Get-FileHash $_.FullName).Hash)  $($_.FullName.Substring($prefix.Length).Replace('\','/'))"})
[IO.File]::WriteAllLines("$dst/SHA256SUMS.txt",[string[]]$sums)
Compress-Archive -Path "$dst/*" -DestinationPath "$dst.zip"
Add-Type -AssemblyName System.IO.Compression.FileSystem
$z=[IO.Compression.ZipFile]::OpenRead("$dst.zip")
try{foreach($line in $sums){
 $hash,$p=$line -split '  ',2;$e=$z.GetEntry("SD_ROOT/$p");if(!$e){$e=$z.GetEntry("SD_ROOT\$($p.Replace('/','\'))")};if(!$e){throw "Missing $p"}
 $s=$e.Open();$h=[Security.Cryptography.SHA256]::Create()
 try{if(([BitConverter]::ToString($h.ComputeHash($s))).Replace('-','') -ne $hash){throw "ZIP mismatch $p"}}finally{$s.Dispose();$h.Dispose()}
}}finally{$z.Dispose()}
if($InstallUsb){
 $device=Get-Content -LiteralPath 'D:/Analogue_Pocket.json' -Raw|ConvertFrom-Json
 if($device.product -ne 'Analogue Pocket'){throw 'Not a Pocket mount'}
 $usbBase=$base
 if($Revision -eq 'auto149'){$usbBase="$root/build/smoke/ChisBurner-dev.148-log"}
 if($Revision -eq 'wait146'){$usbBase="$root/build/smoke/ChisBurner-diag.145-bank"}
 if($Revision -eq 'read145'){$usbBase="$root/build/smoke/ChisBurner-diag.144-read"}
 if($Revision -eq 'read144'){$usbBase="$root/build/smoke/ChisBurner-diag.143-read"}
 foreach($p in @('Assets/chisburner/common/os.bin','Cores/ChisBurner.ChisBurner/chis.rbf_r')){
  if((Get-FileHash -LiteralPath "D:/$p").Hash -ne (Get-FileHash -LiteralPath "$usbBase/SD_ROOT/$p").Hash){throw "USB pair mismatch: $p; no installation"}
 }
 $target='D:/Assets/chisburner/common/cb102.elf'
 $backup="$root/build/usb-backup-141-$Revision"
 if(Test-Path $backup){throw 'Preserve backup'}
 New-Item -ItemType Directory -Path $backup|Out-Null
 Copy-Item -LiteralPath $target -Destination "$backup/cb102.elf"
 $oldHash=(Get-FileHash -LiteralPath $target).Hash
 if((Get-FileHash "$backup/cb102.elf").Hash -ne $oldHash){throw 'Backup failed'}
 Copy-Item -LiteralPath $app -Destination $target -Force
 if((Get-FileHash -LiteralPath $target).Hash -ne (Get-FileHash -LiteralPath $app).Hash){throw 'USB verification failed'}
 if($Revision -in @('read143','read144','read145','wait146','auto149')){
  foreach($p in @('Assets/chisburner/common/os.bin','Cores/ChisBurner.ChisBurner/chis.rbf_r')){
   Copy-Item -LiteralPath "D:/$p" -Destination "$backup/$([IO.Path]::GetFileName($p))"
   if((Get-FileHash "$backup/$([IO.Path]::GetFileName($p))").Hash -ne (Get-FileHash "D:/$p").Hash){throw 'Pair backup failed'}
   Copy-Item -LiteralPath "$dst/SD_ROOT/$p" -Destination "D:/$p" -Force
   if((Get-FileHash "D:/$p").Hash -ne (Get-FileHash "$dst/SD_ROOT/$p").Hash){throw 'Pair copy mismatch'}
  }
 }
 'PASS USB app installed and SHA256 readback verified; old app backed up on PC'
}
'PASS archive payload verification'
