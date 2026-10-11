param([switch]$InstallUsb)
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$base="$root/build/smoke/ChisBurner-diag.146-wait"
$dst="$root/build/smoke/ChisBurner-dev.147-save"
$app="$root/build/save147-app/app.elf"
if((Test-Path $dst)-or(Test-Path "$dst.zip")){throw 'Preserve existing release'}
foreach($line in Get-Content "$base/SHA256SUMS.txt"){
 $hash,$p=$line -split '  ',2
 if((Get-FileHash -LiteralPath "$base/SD_ROOT/$p").Hash -ne $hash){throw "Base mismatch $p"}
}
$strings=[Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($app))
if(!$strings.Contains('dev.147-save') -or $strings.Contains('EEPROM capture') -or $strings.Contains('Save probe + wait')){throw 'Incorrect UI build'}
Copy-Item "$base/SD_ROOT" "$dst/SD_ROOT" -Recurse
Copy-Item $app "$dst/SD_ROOT/Assets/chisburner/common/cb102.elf" -Force
Copy-Item "$root/docs/SAVE147_CN.md" "$dst/README.md"
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
 if((Get-Content D:/Analogue_Pocket.json -Raw|ConvertFrom-Json).product -ne 'Analogue Pocket'){throw 'Wrong USB device'}
 foreach($p in @('Assets/chisburner/common/os.bin','Cores/ChisBurner.ChisBurner/chis.rbf_r','Assets/chisburner/common/cb102.elf')){
  if((Get-FileHash "D:/$p").Hash -ne (Get-FileHash "$base/SD_ROOT/$p").Hash){throw "USB base mismatch $p"}
 }
 $backup="$root/build/usb-backup-save147"
 if(Test-Path $backup){throw 'Preserve backup'}
 New-Item -ItemType Directory $backup|Out-Null
 Copy-Item D:/Assets/chisburner/common/cb102.elf "$backup/cb102.elf"
 if((Get-FileHash "$backup/cb102.elf").Hash -ne (Get-FileHash D:/Assets/chisburner/common/cb102.elf).Hash){throw 'Backup mismatch'}
 Copy-Item $app D:/Assets/chisburner/common/cb102.elf -Force
 if((Get-FileHash $app).Hash -ne (Get-FileHash D:/Assets/chisburner/common/cb102.elf).Hash){throw 'Install mismatch'}
 'PASS USB app installed and SHA256 verified; OS/FPGA unchanged; old app backed up'
}
'PASS ZIP payload SHA256 verification'
