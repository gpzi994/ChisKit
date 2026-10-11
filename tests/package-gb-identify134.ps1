$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$fpga="$root/build/gb-identify134-quartus"
$os="$root/.integration/openfpgaos/src/firmware/os/bld/gpu-lean132-release"
$app="$root/build/ui134-gb-app/app.elf"
$dst="$root/build/smoke/ChisBurner-smoke.134-gb-identify"
function Sha($p){(Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash.ToLowerInvariant()}
if((Test-Path $dst)-or(Test-Path "$dst.zip")){throw 'Preserve previous package'}
if((Sha "$os/firmware.mif") -cne (Sha "$fpga/firmware.mif")){throw 'MIF mismatch'}
if((Get-Content "$fpga/full-compile.log" -Raw) -notmatch 'Full Compilation was successful'){throw 'Compile not successful'}
& "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $fpga
. "$PSScriptRoot/os-image-integrity.ps1"
$null=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes("$os/os.bin"))
if(!([Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($app))).Contains('dev.134-gb')){throw 'Wrong app version'}
$null=New-Item -ItemType Directory -Path $dst
Copy-Item -LiteralPath "$root/build/smoke/ChisBurner-smoke.131-hw-psram-nor-r7/SD_ROOT" -Destination "$dst/SD_ROOT" -Recurse
$core="$dst/SD_ROOT/Cores/ChisBurner.ChisBurner"
$assets="$dst/SD_ROOT/Assets/chisburner/common"
Add-Type -TypeDefinition @'
public static class Identify134Bits { public static byte[] Reverse(byte[] a) { for(int i=0;i<a.Length;i++){int x=a[i],y=0;for(int b=0;b<8;b++){y=(y<<1)|(x&1);x>>=1;}a[i]=(byte)y;}return a;} }
'@
[IO.File]::WriteAllBytes("$core/chis.rbf_r",[Identify134Bits]::Reverse([IO.File]::ReadAllBytes("$fpga/output_files/ap_core.rbf")))
Copy-Item -LiteralPath "$os/os.bin" -Destination "$assets/os.bin" -Force
Copy-Item -LiteralPath $app -Destination "$assets/cb102.elf" -Force
$utf8=[Text.UTF8Encoding]::new($false)
$meta=Get-Content "$core/core.json" -Raw|ConvertFrom-Json
$meta.core.metadata.version='0.1.0-smoke.134-gb-identify'
[IO.File]::WriteAllText("$core/core.json",($meta|ConvertTo-Json -Depth 20),$utf8)
Copy-Item "$root/docs/SMOKE134_GB_IDENTIFY_TEST_CN.md" "$dst/TEST_CN.md"
Copy-Item "$fpga/output_files/ap_core.fit.summary" "$dst/FIT_SUMMARY.txt"
Copy-Item "$fpga/output_files/ap_core.sta.summary" "$dst/TIMING_SUMMARY.txt"
[IO.File]::WriteAllText("$dst/README.txt","dev.134-gb: install COMPLETE SD_ROOT. Read TEST_CN.md. Hardware validation pending. Constrained timing/CDC passed; external I/O constraints remain incomplete. Start with boot/UI/file selection and ROM backup. PSRAM remains ROM-only.`r`n",$utf8)
$inputs=@($app,"$os/os.bin","$os/firmware.mif","$fpga/output_files/ap_core.rbf","$fpga/ap_core.qsf","$fpga/full-compile.log")
[IO.File]::WriteAllLines("$dst/BUILD_INPUTS_SHA256.txt",[string[]]@($inputs|ForEach-Object{"$(Sha $_)  $_"}),$utf8)
$prefix="$dst/SD_ROOT/"
$sums=@(Get-ChildItem "$dst/SD_ROOT" -Recurse -File|Sort-Object FullName|ForEach-Object{"$(Sha $_.FullName)  $($_.FullName.Substring($prefix.Length).Replace('\','/'))"})
[IO.File]::WriteAllLines("$dst/SHA256SUMS.txt",[string[]]$sums,$utf8)
Compress-Archive -Path "$dst/*" -DestinationPath "$dst.zip"
Add-Type -AssemblyName System.IO.Compression.FileSystem
$z=[IO.Compression.ZipFile]::OpenRead("$dst.zip")
try {foreach($line in $sums){
 $h,$f=$line -split '  ',2
 $e=$z.GetEntry("SD_ROOT/$f")
 if(!$e){$e=$z.GetEntry("SD_ROOT\$($f.Replace('/','\'))")}
 if(!$e){throw "Missing $f"}
 $s=$e.Open();$a=[Security.Cryptography.SHA256]::Create()
 try{$actual=([BitConverter]::ToString($a.ComputeHash($s))).Replace('-','').ToLowerInvariant()}finally{$s.Dispose();$a.Dispose()}
 if($actual -cne $h){throw "ZIP mismatch $f"}
}}finally{$z.Dispose()}
'PASS ZIP hashes'
Get-FileHash "$dst.zip"
