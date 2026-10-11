param([int]$Seed=4,[string]$Variant='floorplan5control',[ValidateSet(108,109,110,111)][int]$Revision=108)
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
Set-Location $root
function Sha($p){(Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash.ToLowerInvariant()}
function Check-Receipt($receipt){
 foreach($line in Get-Content -LiteralPath $receipt){
  if($line -notmatch '^([0-9a-f]{64})  (.+)$'){throw "Invalid receipt $receipt"}
  $digest=$Matches[1];$path=$Matches[2]
  if($path.StartsWith('/mnt/c/')){$path='C:/'+$path.Substring(7)}
  elseif(-not[IO.Path]::IsPathRooted($path)){$path=Join-Path $root $path}
  if((Sha $path) -cne $digest){throw "Build input changed: $path"}
 }
}
$base=Join-Path $root 'build/smoke/ChisBurner-smoke.107-mapper-dump-r1'
$bundle=Join-Path $root "build/smoke/ChisBurner-smoke.$Revision-nor-test-r1"
$suffix=if($Variant -eq 'standard'){''}else{"-$Variant"}
$fpga=Join-Path $root ".integration/openfpgaos/src/fpga/targets/pocket/bld/chisburner-ui$Revision-s$Seed$suffix-win"
$osBuild=if($Revision -ge 110){"ui$Revision-release-pocket"}elseif($Revision -eq 109){'ui109-board-pocket'}else{'ui108-pocket'}
$os=Join-Path $root ".integration/openfpgaos/src/firmware/os/bld/$osBuild"
$app=if($Revision -ge 110){"build/ui$Revision-release-app"}elseif($Revision -eq 109){'build/ui109-release-app'}else{'build/ui108-app'}
if((Test-Path $bundle)-or(Test-Path "$bundle.zip")){throw 'Preserve existing package'}
Check-Receipt "$app/sources.sha256"
Check-Receipt "$os/OS_INPUTS_SHA256.txt"
if($Revision -ge 111){
 $wroot='/mnt/'+$root.Substring(0,1).ToLowerInvariant()+$root.Substring(2).Replace('\','/')
 & wsl.exe -d Ubuntu-24.04 -- bash -lc "cd '$wroot' && bash tests/check-nor111-records.sh"
 if($LASTEXITCODE){throw 'Database coverage audit failed; do not package'}
}
foreach($line in Get-Content "$base/SHA256SUMS.txt"){
 if($line -notmatch '^([0-9a-f]{64})  (.+)$'){throw 'Bad baseline manifest'}
 $digest=$Matches[1];$path=$Matches[2]
 if((Sha "$base/SD_ROOT/$path") -cne $digest){throw "Baseline changed: $path"}
}
$changed=@('core/rtl/chisburner_cart_auto98.v','.integration/openfpgaos/src/fpga/targets/pocket/core_top.v','.integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gb_mbc5_fullrom.v','.integration/openfpgaos/src/firmware/os/kernel/services_table.c')
foreach($line in Get-Content 'build/smoke/ChisBurner-smoke.101-transfer-mapper-fix/SOURCES_SHA256.txt'){
 if($line -notmatch '^([0-9a-f]{64})  (.+)$'){throw 'Bad protected source receipt'}
 $digest=$Matches[1];$path=$Matches[2]
 if(($path.StartsWith('.integration/')-or $path.StartsWith('core/rtl/')-or $path -eq 'app/gba_program65/program65.h')-and $path -notin $changed){
  if((Sha "$root/$path") -cne $digest){throw "Protected source changed: $path"}
 }
}
& "$PSScriptRoot/check-ui108-fpga-inputs.ps1" -Seed $Seed -Variant $Variant -Revision $Revision
if((Sha "$fpga/firmware.mif") -cne (Sha "$os/firmware.mif")){throw 'Embedded OS mismatch'}
$timing=@();$sta=Get-Content "$fpga/output_files/ap_core.sta.rpt" -Raw
foreach($kind in @('setup','hold')){
 $matchesFound=[regex]::Matches($sta,"Worst-case $kind slack is\s+(-?\d+(?:\.\d+)?)")
 if(!$matchesFound.Count){throw "Missing $kind timing"}
 $minimum=($matchesFound|ForEach-Object{[double]$_.Groups[1].Value}|Measure-Object -Minimum).Minimum
 if($minimum -le 0){throw "$kind slack $minimum; do not ship"}
 $timing+="$kind slack: $minimum ns"
}
. "$PSScriptRoot/os-image-integrity.ps1"
$null=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes("$os/os.bin"))
if(-not('ChisBurner.Ui108.Reverse'-as[type])){Add-Type -TypeDefinition @'
namespace ChisBurner.Ui108 { public static class Reverse { public static byte[] Apply(byte[] i) { var t=new byte[256]; for(int n=0;n<256;n++){int x=n,y=0;for(int b=0;b<8;b++){y=(y<<1)|(x&1);x>>=1;}t[n]=(byte)y;}var o=new byte[i.Length];for(int n=0;n<i.Length;n++)o[n]=t[i[n]];return o; } } }
'@}
$utf8=[Text.UTF8Encoding]::new($false)
New-Item -ItemType Directory -Path $bundle | Out-Null
Copy-Item -LiteralPath "$base/SD_ROOT" -Destination "$bundle/SD_ROOT" -Recurse
Copy-Item "$app/app.elf" "$bundle/SD_ROOT/Assets/chisburner/common/cb102.elf"
Copy-Item "$os/os.bin" "$bundle/SD_ROOT/Assets/chisburner/common/os.bin"
$core="$bundle/SD_ROOT/Cores/ChisBurner.ChisBurner"
[IO.File]::WriteAllBytes("$core/chis.rbf_r",[ChisBurner.Ui108.Reverse]::Apply([IO.File]::ReadAllBytes("$fpga/output_files/ap_core.rbf")))
$meta=Get-Content "$core/core.json" -Raw|ConvertFrom-Json
$meta.core.metadata.version="0.1.0-smoke.$Revision-nor-test"
[IO.File]::WriteAllText("$core/core.json",($meta|ConvertTo-Json -Depth 20),$utf8)
$data=Get-Content "$core/data.json" -Raw|ConvertFrom-Json
$inputSlot=@($data.data.data_slots|Where-Object id -eq 4)
if($inputSlot.Count -ne 1){throw 'Missing unique ROM input slot'}
$inputSlot[0].extensions=@('gba','gb','gbc')
[IO.File]::WriteAllText("$core/data.json",($data|ConvertTo-Json -Depth 20),$utf8)
[IO.File]::WriteAllText("$core/info.txt","smoke.$Revision NOR hardware TEST: dynamic ID/CFI, erase/program/readback, fresh-ID two-A gate. Not all records hardware validated. Special board mapping remains limited. Read TEST_CN.md.",$utf8)
Copy-Item "docs/SMOKE${Revision}_TEST_CN.md" "$bundle/TEST_CN.md"
Copy-Item "docs/SMOKE${Revision}_VERIFICATION.md" "$bundle/VERIFICATION.md"
if($Revision -ge 111){
 Copy-Item 'build/nor111/records.tsv' "$bundle/NOR_RECORDS.tsv"
 Copy-Item 'docs/NOR111_PROGRESS.md' "$bundle/ADAPTATION_LIMITS.md"
}
Copy-Item "$fpga/UI98_INPUTS_SHA256.txt" "$bundle/FPGA_INPUTS_SHA256.txt"
Copy-Item "$os/OS_INPUTS_SHA256.txt" "$bundle/OS_INPUTS_SHA256.txt"
Copy-Item "$app/sources.sha256" "$bundle/APP_INPUTS_SHA256.txt"
[IO.File]::WriteAllText("$bundle/README.txt",@"
ChisBurner smoke.$Revision NOR TEST package. Install all SD_ROOT contents, not only the app.
Read TEST_CN.md before writing. Back up ROM first. New algorithms require hardware testing.
60 imported Mori records are NOT 60 hardware-qualified cartridge boards. Unsupported/ambiguous
geometry and special mappings remain locked; this is not a claim of full database hardware coverage.
Full editable project and build inputs are in: $root
Do not redistribute this local development bundle as a stable public release.
Mori-derived NOR sources retain AGPL-3.0-only and attribution. No game ROM is bundled.
$($timing -join "`r`n")
"@,$utf8)
$sourceFiles=@(Get-ChildItem core/nor,core/rtl,app/ui98 -Recurse -File | ForEach-Object FullName)
$sourceFiles+=@("$root/.integration/openfpgaos/src/firmware/api/of_nor.h","$root/.integration/openfpgaos/src/firmware/api/of_services.h","$root/.integration/openfpgaos/src/firmware/os/kernel/nor108.c","$root/.integration/openfpgaos/src/firmware/os/kernel/nor108.h")
$sourceFiles+=@(Get-ChildItem tests -File | Where-Object {$_.Name -like '*108*' -or ($Revision -ge 109 -and $_.Name -like '*109*') -or ($Revision -ge 110 -and $_.Name -like '*110*')} | ForEach-Object FullName)
if($Revision -ge 111){$sourceFiles+=@(Get-ChildItem tests -File | Where-Object {$_.Name -like '*111*'} | ForEach-Object FullName)}
$receipt=@()
foreach($path in $sourceFiles|Sort-Object -Unique){
 $rel=[IO.Path]::GetRelativePath($root,$path);$dest=Join-Path "$bundle/DEVELOPMENT_SOURCE" $rel
 New-Item -ItemType Directory -Path (Split-Path $dest) -Force | Out-Null
 Copy-Item -LiteralPath $path -Destination $dest
 $receipt+="$(Sha $path)  $($rel.Replace('\','/'))"
}
[IO.File]::WriteAllLines("$bundle/SOURCES_SHA256.txt",[string[]]$receipt,$utf8)
$prefix=[IO.Path]::GetFullPath("$bundle/SD_ROOT").TrimEnd('\')+'\'
$sums=@(Get-ChildItem "$bundle/SD_ROOT" -Recurse -File|Sort-Object FullName|ForEach-Object{"$(Sha $_.FullName)  $($_.FullName.Substring($prefix.Length).Replace('\','/'))"})
[IO.File]::WriteAllLines("$bundle/SHA256SUMS.txt",[string[]]$sums,$utf8)
Compress-Archive -Path "$bundle/*" -DestinationPath "$bundle.zip"
if($Revision -ge 110){
 & "$PSScriptRoot/verify-nor110-package.ps1" -ZipPath "$bundle.zip" -Revision $Revision
}
$null=Assert-ChisBurnerZipOsIntegrity -ZipPath "$bundle.zip"
Get-FileHash "$bundle.zip" -Algorithm SHA256
