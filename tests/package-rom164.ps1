$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$base=Join-Path $root 'build/smoke/ChisBurner-dev.163-size-r2'
$dst=Join-Path $root 'build/smoke/ChisBurner-dev.164-rom-r1'
$app=Join-Path $root 'build/rom164-app/app.elf'
function Sha([string]$p){(Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash.ToUpperInvariant()}
if((Test-Path -LiteralPath $dst)-or(Test-Path -LiteralPath "$dst.zip")){throw 'Preserve existing package'}
foreach($line in Get-Content "$base/SHA256SUMS.txt"){
 $hash,$p=$line -split '  ',2
 if((Sha "$base/SD_ROOT/$p") -cne $hash.ToUpperInvariant()){throw "Base mismatch: $p"}
}
$strings=[Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($app))
foreach($phrase in @('dev.164-rom','Cleared; select Analyze Cart','Database ROM capacity backup','NOR capacity backup','Write EEPROM SAV')){
 if(!$strings.Contains($phrase)){throw "App missing $phrase"}
}
$null=New-Item -ItemType Directory -Path $dst
Copy-Item -LiteralPath "$base/SD_ROOT" -Destination "$dst/SD_ROOT" -Recurse
$core="$dst/SD_ROOT/Cores/ChisBurner.ChisBurner"
$assets="$dst/SD_ROOT/Assets/chisburner/common"
Copy-Item -LiteralPath $app -Destination "$assets/cb102.elf" -Force
$utf8=[Text.UTF8Encoding]::new($false)
$meta=Get-Content "$core/core.json" -Raw|ConvertFrom-Json
$meta.core.metadata.version='0.1.0-dev.164-rom-r1'
[IO.File]::WriteAllText("$core/core.json",($meta|ConvertTo-Json -Depth 20),$utf8)
foreach($relative in @('Assets/chisburner/common/os.bin','Cores/ChisBurner.ChisBurner/chis.rbf_r')){
 if((Sha "$base/SD_ROOT/$relative") -cne (Sha "$dst/SD_ROOT/$relative")){throw "Firmware changed: $relative"}
}
Copy-Item "$root/docs/ROM164_TEST_CN.md" "$dst/TEST_CN.md"
$null=New-Item -ItemType Directory -Path "$dst/Source/app","$dst/Source/tests","$dst/ThirdParty/FlashGBX" -Force
foreach($dir in @('ui98','ui97','gba_db55')){Copy-Item "$root/app/$dir" "$dst/Source/app/$dir" -Recurse}
foreach($file in @('test-rom164.c','check-rom164.sh','package-rom164.ps1')){Copy-Item "$root/tests/$file" "$dst/Source/tests/$file"}
Copy-Item "$root/data/chisburner/db_AGB.json" "$dst/ThirdParty/FlashGBX/db_AGB.json"
Copy-Item "$root/references/upstream/lesserkuma-flash-database/LICENSE" "$dst/ThirdParty/FlashGBX/LICENSE"
Copy-Item "$root/docs/ROM164_DATABASE_NOTICE.md" "$dst/ThirdParty/FlashGBX/NOTICE.md"
Copy-Item "$root/build/rom164-build.log" "$dst/BUILD.log"
Copy-Item "$root/build/rom164-app/sources.sha256" "$dst/SOURCES_SHA256.txt"
$prefix=[IO.Path]::GetFullPath("$dst/SD_ROOT")+'\'
$sums=@(Get-ChildItem "$dst/SD_ROOT" -Recurse -File|Sort-Object FullName|ForEach-Object{
 "$(Sha $_.FullName)  $($_.FullName.Substring($prefix.Length).Replace('\','/'))"
})
[IO.File]::WriteAllLines("$dst/SHA256SUMS.txt",[string[]]$sums,$utf8)
Compress-Archive -Path "$dst/*" -DestinationPath "$dst.zip"
Add-Type -AssemblyName System.IO.Compression.FileSystem
$zip=[IO.Compression.ZipFile]::OpenRead("$dst.zip")
try{foreach($line in $sums){
 $hash,$relative=$line -split '  ',2
 $entry=$zip.GetEntry("SD_ROOT/$relative")
 if(!$entry){$entry=$zip.GetEntry("SD_ROOT\$($relative.Replace('/','\'))")}
 if(!$entry){throw "ZIP missing: $relative"}
 $stream=$entry.Open();$sha=[Security.Cryptography.SHA256]::Create()
 try{$actual=([BitConverter]::ToString($sha.ComputeHash($stream))).Replace('-','');if($actual -cne $hash){throw "ZIP mismatch: $relative"}}
 finally{$stream.Dispose();$sha.Dispose()}
}}finally{$zip.Dispose()}
Write-Host "PASS package: $dst.zip"
Get-FileHash -LiteralPath "$dst.zip" -Algorithm SHA256
