$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$base="$root/build/smoke/ChisBurner-smoke.135-timers"
$dst="$root/build/smoke/ChisBurner-smoke.136-camera-diag"
$app="$root/build/camera136-app/app.elf"
function Sha($p){(Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash}
if((Test-Path $dst)-or(Test-Path "$dst.zip")){throw 'Preserve existing package'}
if(!([Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($app))).Contains('diag.136-cam')){throw 'Wrong application'}
New-Item -ItemType Directory -Path $dst | Out-Null
Copy-Item -LiteralPath "$base/SD_ROOT" -Destination "$dst/SD_ROOT" -Recurse
Copy-Item -LiteralPath $app -Destination "$dst/SD_ROOT/Assets/chisburner/common/cb102.elf" -Force
foreach($rel in @('Cores/ChisBurner.ChisBurner/chis.rbf_r','Assets/chisburner/common/os.bin')){
 if((Sha "$base/SD_ROOT/$rel") -cne (Sha "$dst/SD_ROOT/$rel")){throw "Changed $rel"}
}
Copy-Item -LiteralPath "$root/docs/SMOKE136_CAMERA_TEST_CN.md" -Destination "$dst/TEST_CN.md"
Copy-Item -LiteralPath "$root/build/camera136-app/sources.sha256" -Destination "$dst/BUILD_SOURCES.sha256"
$prefix="$dst/SD_ROOT/"
$sums=@(Get-ChildItem "$dst/SD_ROOT" -Recurse -File|Sort-Object FullName|ForEach-Object{"$(Sha $_.FullName)  $($_.FullName.Substring($prefix.Length).Replace('\','/'))"})
$sums | Set-Content -LiteralPath "$dst/SHA256SUMS.txt" -Encoding utf8
Compress-Archive -Path "$dst/*" -DestinationPath "$dst.zip"
Add-Type -AssemblyName System.IO.Compression.FileSystem
$z=[IO.Compression.ZipFile]::OpenRead("$dst.zip")
try{foreach($line in $sums){
 $h,$f=$line -split '  ',2
 $e=$z.GetEntry("SD_ROOT/$f")
 if(!$e){$e=$z.GetEntry("SD_ROOT\$($f.Replace('/','\'))")}
 if(!$e){throw "Missing $f"}
 $s=$e.Open();$a=[Security.Cryptography.SHA256]::Create()
 try{$actual=([BitConverter]::ToString($a.ComputeHash($s))).Replace('-','')}finally{$s.Dispose();$a.Dispose()}
 if($actual -cne $h){throw "Hash mismatch $f"}
}}finally{$z.Dispose()}
'PASS ZIP hashes; FPGA and OS identical to 135'
Get-FileHash -LiteralPath "$dst.zip"
