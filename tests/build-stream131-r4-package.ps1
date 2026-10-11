param([ValidateSet('r4','r5','r6','r7')][string]$Revision='r4')
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$target="$root/build/smoke/ChisBurner-smoke.131-hw-psram-nor-$Revision"
$runtime=if($Revision -eq 'r7'){'r6'}else{$Revision}
$fpga="$root/build/stream131-$runtime-quartus"
$os="$root/.integration/openfpgaos/src/firmware/os/bld/stream131-$runtime-release"
$app="$root/build/ui131-$Revision-app/app.elf"
function Sha($p){(Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash.ToLowerInvariant()}
if((Test-Path $target)-or(Test-Path "$target.zip")){throw 'Preserve existing r4'}
if((Sha "$os/firmware.mif") -cne (Sha "$fpga/firmware.mif")){throw 'MIF mismatch'}
foreach($pair in @(@('mif-update.log','MIF/HEX Update was successful'),@('assembler.log','Assembler was successful'))){
 if((Get-Content "$fpga/$($pair[0])" -Raw) -notmatch $pair[1]){throw "Failed $($pair[0])"}
}
. "$PSScriptRoot/os-image-integrity.ps1"
$null=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes("$os/os.bin"))
if(!([Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($app))).Contains("smoke.131-$Revision")){throw 'Wrong app'}
Copy-Item -LiteralPath "$root/build/smoke/ChisBurner-smoke.131-hw-psram-nor-r3" -Destination $target -Recurse
$core="$target/SD_ROOT/Cores/ChisBurner.ChisBurner"
$assets="$target/SD_ROOT/Assets/chisburner/common"
Add-Type -TypeDefinition @'
public static class Stream131R4Bits { public static byte[] Reverse(byte[] a) { for(int i=0;i<a.Length;i++){int x=a[i],y=0;for(int b=0;b<8;b++){y=(y<<1)|(x&1);x>>=1;}a[i]=(byte)y;}return a;} }
'@
[IO.File]::WriteAllBytes("$core/chis.rbf_r",[Stream131R4Bits]::Reverse([IO.File]::ReadAllBytes("$fpga/output_files/ap_core.rbf")))
Copy-Item "$os/os.bin" "$assets/os.bin" -Force
Copy-Item $app "$assets/cb102.elf" -Force
$utf8=[Text.UTF8Encoding]::new($false)
$meta=Get-Content "$core/core.json" -Raw|ConvertFrom-Json
$meta.core.metadata.version="0.1.0-smoke.131-hw-psram-nor-$Revision"
[IO.File]::WriteAllText("$core/core.json",($meta|ConvertTo-Json -Depth 20),$utf8)
Copy-Item "$root/docs/SMOKE131_R4_TEST_CN.md" "$target/R4_TEST_CN.md"
if($Revision -eq 'r5'){Copy-Item "$root/docs/SMOKE131_R5_TEST_CN.md" "$target/R5_TEST_CN.md"}
if($Revision -eq 'r6'){Copy-Item "$root/docs/SMOKE131_R6_TEST_CN.md" "$target/R6_TEST_CN.md"}
if($Revision -eq 'r7'){Copy-Item "$root/docs/SMOKE131_R7_TEST_CN.md" "$target/R7_TEST_CN.md"}
$receipt=@("$Revision replaces OS, boot MIF and application; FPGA routing unchanged. Historical receipts describe prior packages only.")
foreach($p in @($app,"$os/os.bin","$os/firmware.mif","$fpga/output_files/ap_core.rbf","$fpga/mif-update.log","$fpga/assembler.log","$root/app/ui98/main.c","$root/app/ui98/ui.c","$root/.integration/openfpgaos/src/firmware/os/kernel/nor108.c")){$receipt+="$(Sha $p)  $p"}
[IO.File]::WriteAllLines("$target/$($Revision.ToUpper())_BUILD_INPUTS_SHA256.txt",[string[]]$receipt,$utf8)
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
