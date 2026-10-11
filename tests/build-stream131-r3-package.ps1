$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$target="$root/build/smoke/ChisBurner-smoke.131-hw-psram-nor-r3"
if((Test-Path $target)-or(Test-Path "$target.zip")){throw 'Preserve existing r3'}
$app="$root/build/ui131-r3-app/app.elf"
if(!([Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($app))).Contains('smoke.131-r3')){throw 'Wrong app version'}
Copy-Item -LiteralPath "$root/build/smoke/ChisBurner-smoke.131-hw-psram-nor-r2" -Destination $target -Recurse
Copy-Item $app "$target/SD_ROOT/Assets/chisburner/common/cb102.elf" -Force
$core="$target/SD_ROOT/Cores/ChisBurner.ChisBurner/core.json"
$meta=Get-Content $core -Raw|ConvertFrom-Json
$meta.core.metadata.version='0.1.0-smoke.131-hw-psram-nor-r3'
$utf8=[Text.UTF8Encoding]::new($false)
[IO.File]::WriteAllText($core,($meta|ConvertTo-Json -Depth 20),$utf8)
Copy-Item "$root/docs/SMOKE131_R3_FIX_CN.md" "$target/R3_FIX_CN.md"
function Sha($p){(Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash.ToLowerInvariant()}
$receipt=@('R3 replaces only the application and core version metadata. FPGA and OS are identical to r2.')
foreach($p in @($app,"$root/app/ui98/transfer.c","$root/app/ui98/version.h","$root/tests/test-ui98-transfer.c")){$receipt+="$(Sha $p)  $p"}
[IO.File]::WriteAllLines("$target/R3_BUILD_INPUTS_SHA256.txt",[string[]]$receipt,$utf8)
$prefix="$target/SD_ROOT/"
$sums=@(Get-ChildItem "$target/SD_ROOT" -Recurse -File|Sort-Object FullName|ForEach-Object{"$(Sha $_.FullName)  $($_.FullName.Substring($prefix.Length).Replace('\','/'))"})
[IO.File]::WriteAllLines("$target/SHA256SUMS.txt",[string[]]$sums,$utf8)
Compress-Archive -Path "$target/*" -DestinationPath "$target.zip"
Add-Type -AssemblyName System.IO.Compression.FileSystem
$z=[IO.Compression.ZipFile]::OpenRead("$target.zip")
try {foreach($line in $sums){
 $h,$f=$line -split '  ',2;$e=$z.GetEntry("SD_ROOT/$f")
 if(!$e){$e=$z.GetEntry("SD_ROOT\$($f.Replace('/','\'))")};if(!$e){throw "Missing $f"}
 $s=$e.Open();$a=[Security.Cryptography.SHA256]::Create()
 try{$actual=([BitConverter]::ToString($a.ComputeHash($s))).Replace('-','').ToLowerInvariant()}finally{$s.Dispose();$a.Dispose()}
 if($actual -cne $h){throw "Mismatch $f"}
}}finally{$z.Dispose()}
'PASS ZIP SD_ROOT hashes'
Get-FileHash "$target.zip"
