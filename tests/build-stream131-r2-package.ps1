$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
Set-Location $root
$source="$root/build/smoke/ChisBurner-smoke.131-hw-psram-nor-r1"
$target="$root/build/smoke/ChisBurner-smoke.131-hw-psram-nor-r2"
$fpga="$root/build/stream131-r2-quartus"
$os="$root/.integration/openfpgaos/src/firmware/os/bld/stream131-r2-release"
function Sha($p){(Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash.ToLowerInvariant()}
if((Test-Path -LiteralPath $target)-or(Test-Path -LiteralPath "$target.zip")){throw 'Preserve existing r2'}
if((Sha "$os/firmware.mif") -cne (Sha "$fpga/firmware.mif")){throw 'Boot MIF mismatch'}
foreach($pair in @(@('mif-update.log','MIF/HEX Update was successful'),@('assembler.log','Assembler was successful'))){
 if((Get-Content "$fpga/$($pair[0])" -Raw) -notmatch $pair[1]){throw "Failed $($pair[0])"}
}
. "$PSScriptRoot/os-image-integrity.ps1"
$null=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes("$os/os.bin"))
Copy-Item -LiteralPath $source -Destination $target -Recurse
$core="$target/SD_ROOT/Cores/ChisBurner.ChisBurner"
$assets="$target/SD_ROOT/Assets/chisburner/common"
Add-Type -TypeDefinition @'
public static class Stream131R2Bits { public static byte[] Reverse(byte[] a) { for(int i=0;i<a.Length;i++){int x=a[i], y=0;for(int b=0;b<8;b++){y=(y<<1)|(x&1);x>>=1;}a[i]=(byte)y;}return a;} }
'@
[IO.File]::WriteAllBytes("$core/chis.rbf_r",[Stream131R2Bits]::Reverse([IO.File]::ReadAllBytes("$fpga/output_files/ap_core.rbf")))
Copy-Item "$os/os.bin" "$assets/os.bin" -Force
$utf8=[Text.UTF8Encoding]::new($false)
$meta=Get-Content "$core/core.json" -Raw|ConvertFrom-Json
$meta.core.metadata.version='0.1.0-smoke.131-hw-psram-nor-r2'
[IO.File]::WriteAllText("$core/core.json",($meta|ConvertTo-Json -Depth 20),$utf8)
Copy-Item "$root/docs/SMOKE131_R2_FIX_CN.md" "$target/R2_FIX_CN.md"
$receipt=@('R2 supersedes the OS and boot image in the inherited r1 build receipt. FPGA routing and application are unchanged.')
foreach($p in @("$os/os.bin","$os/firmware.mif","$os/firmware.elf","$fpga/output_files/ap_core.rbf","$fpga/mif-update.log","$fpga/assembler.log","$root/tests/build-stream131-os.sh","$root/tests/check-stream131-caps.py")){$receipt+="$(Sha $p)  $p"}
[IO.File]::WriteAllLines("$target/R2_BUILD_INPUTS_SHA256.txt",[string[]]$receipt,$utf8)
$prefix="$target/SD_ROOT/"
$sums=@(Get-ChildItem "$target/SD_ROOT" -Recurse -File|Sort-Object FullName|ForEach-Object{"$(Sha $_.FullName)  $($_.FullName.Substring($prefix.Length).Replace('\','/'))"})
[IO.File]::WriteAllLines("$target/SHA256SUMS.txt",[string[]]$sums,$utf8)
Compress-Archive -Path "$target/*" -DestinationPath "$target.zip"
Add-Type -AssemblyName System.IO.Compression.FileSystem
$zip=[IO.Compression.ZipFile]::OpenRead("$target.zip")
try { foreach($line in $sums){
 $hash,$path=$line -split '  ',2
 $entry=$zip.GetEntry("SD_ROOT/$path")
 if(!$entry){$entry=$zip.GetEntry("SD_ROOT\$($path.Replace('/','\'))")}
 if(!$entry){throw "Missing $path"}
 $s=$entry.Open();$sha=[Security.Cryptography.SHA256]::Create()
 try{$actual=([BitConverter]::ToString($sha.ComputeHash($s))).Replace('-','').ToLowerInvariant()}finally{$s.Dispose();$sha.Dispose()}
 if($actual -cne $hash){throw "ZIP mismatch $path"}
}} finally {$zip.Dispose()}
Get-FileHash "$target.zip"
