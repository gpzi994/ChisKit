$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
Set-Location $root
function Sha($p){(Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash.ToLowerInvariant()}
$base=Join-Path $root 'build/smoke/ChisBurner-smoke.105-lv640-readonly'
$bundle=Join-Path $root 'build/smoke/ChisBurner-smoke.106-gb-dump-r1'
$fpga=Join-Path $root '.integration/openfpgaos/src/fpga/targets/pocket/bld/chisburner-ui106-s3-floorplan5control-win'
$os=Join-Path $root '.integration/openfpgaos/src/firmware/os/bld/ui106-pocket'
if((Test-Path $bundle)-or(Test-Path "$bundle.zip")){throw 'Preserve existing package'}
foreach($line in Get-Content "$base/SHA256SUMS.txt"){
 if($line -notmatch '^([0-9a-f]{64})  (.+)$'){throw 'Bad manifest'}
 if((Sha "$base/SD_ROOT/$($Matches[2])") -cne $Matches[1]){throw 'Baseline changed'}
}
$changed=@('core/rtl/chisburner_cart_auto98.v','.integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gb_mbc5_fullrom.v','.integration/openfpgaos/src/firmware/os/kernel/services_table.c')
foreach($line in Get-Content 'build/smoke/ChisBurner-smoke.101-transfer-mapper-fix/SOURCES_SHA256.txt'){
 if($line -notmatch '^([0-9a-f]{64})  (.+)$'){throw 'Bad source receipt'}
 $digest=$Matches[1];$path=$Matches[2]
 if(($path.StartsWith('.integration/')-or $path.StartsWith('core/rtl/')-or $path -eq 'app/gba_program65/program65.h')-and $path -notin $changed){
  if((Sha "$root/$path") -cne $digest){throw "Protected source changed: $path"}
 }
}
& "$PSScriptRoot/check-ui106-fpga-inputs.ps1" -Seed 3 -Variant floorplan5control
if((Sha "$fpga/firmware.mif") -cne (Sha "$os/firmware.mif")){throw 'Embedded OS mismatch'}
$sta=Get-Content "$fpga/output_files/ap_core.sta.rpt" -Raw
$timing=@()
foreach($kind in @('setup','hold')){
 $m=[regex]::Matches($sta,"Worst-case $kind slack is\s+(-?\d+(?:\.\d+)?)")
 if(!$m.Count){throw "Missing $kind timing"}
 $v=($m|ForEach-Object{[double]$_.Groups[1].Value}|Measure-Object -Minimum).Minimum
 if($v -le 0){throw "$kind slack $v"}
 $timing+="$kind slack: $v ns"
}
. "$PSScriptRoot/os-image-integrity.ps1"
$null=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes("$os/os.bin"))
if(-not('ChisBurner.Ui106.Reverse'-as[type])){Add-Type -TypeDefinition @'
namespace ChisBurner.Ui106 { public static class Reverse { public static byte[] Apply(byte[] i) { var t=new byte[256]; for(int n=0;n<256;n++){int x=n,y=0;for(int b=0;b<8;b++){y=(y<<1)|(x&1);x>>=1;}t[n]=(byte)y;}var o=new byte[i.Length];for(int n=0;n<i.Length;n++)o[n]=t[i[n]];return o; } } }
'@}
New-Item -ItemType Directory -Path $bundle | Out-Null
Copy-Item -LiteralPath "$base/SD_ROOT" -Destination "$bundle/SD_ROOT" -Recurse
Copy-Item 'build/ui106-app/app.elf' "$bundle/SD_ROOT/Assets/chisburner/common/cb102.elf"
Copy-Item "$os/os.bin" "$bundle/SD_ROOT/Assets/chisburner/common/os.bin"
$core="$bundle/SD_ROOT/Cores/ChisBurner.ChisBurner"
[IO.File]::WriteAllBytes("$core/chis.rbf_r",[ChisBurner.Ui106.Reverse]::Apply([IO.File]::ReadAllBytes("$fpga/output_files/ap_core.rbf")))
$utf8=[Text.UTF8Encoding]::new($false)
$meta=Get-Content "$core/core.json" -Raw|ConvertFrom-Json
$meta.core.metadata.version='0.1.0-smoke.106-gb-dump'
[IO.File]::WriteAllText("$core/core.json",($meta|ConvertTo-Json -Depth 20),$utf8)
$data=Get-Content "$core/data.json" -Raw|ConvertFrom-Json
$outputSlot=@($data.data.data_slots|Where-Object id -eq 5)
if($outputSlot.Count -ne 1){throw 'Missing unique output slot'}
$outputSlot[0].extensions=@('gba','gb','gbc')
[IO.File]::WriteAllText("$core/data.json",($data|ConvertTo-Json -Depth 20),$utf8)
[IO.File]::WriteAllText("$core/info.txt",'smoke.106: bounded 2 MiB MBC30/MBC5 ROM dump, reference and output SHA256. New OS and FPGA required. GB ROM/save writes disabled.',$utf8)
New-Item -ItemType Directory -Path "$bundle/SD_ROOT/Saves/chisburner/common" -Force | Out-Null
[IO.File]::WriteAllText("$bundle/README.txt",@"
ChisBurner smoke.106 - full 2 MiB game ROM acceptance dumps
Install ALL SD_ROOT contents into SD root, including new os.bin and chis.rbf_r. Do not update only the app. Preserve existing Saves and backups. Loader filename cb102.elf is intentional.
Two exact local references only; no game ROM data included:
MBC30: Pocket Monsters - Crystal Version (J).gbc, 2097152 bytes
SHA256 136ada06cb68656b7de475fa4b278d37dbeff8f5257e7dfdf7f4a4aec19a90f3
MBC5: mengjiandao.gbc, 2097152 bytes
SHA256 8ba533be5908305a0f2f04117a858a91d4098ed8d4d2be6e48c3c3e9df1885e7
For each cart: Refresh, select Backup ROM, press A and confirm with A. No source-file selection is needed. A matching header is required before backup, but is not proof of a complete ROM match.
PASS rc 0 requires all 128 banks read, file reread SHA256 equal to the cartridge stream, and SHA256 equal to the original reference above. Output is under Saves/chisburner/common/cb106-crystal-*.gbc or cb106-zelda-*.gbc. Test each twice, then run the dumped files in your emulator. Send the output files/path if any mismatch occurs.
This is the full 2 MiB GAME ROM, not a full 8 MiB NOR dump or validation of MBC30 high banks. Mapper selection follows Mori: fixed bank 0 at 0000-3FFF, bank 1 onward at 4000-7FFF. Only ROM-bank register writes occur. No GB NOR erase/program, SRAM or RTC writes. B cancels at a read-chunk boundary; incomplete/mismatching output is retained and must not be treated as a valid backup.
The NOR/probe warning does not block this reference-checked read-only dump. It does not authorize writing.
GBA engine sources remain unchanged; shared router/OS and FPGA image are updated. Recheck GBA refresh/analyze and a backup as a regression test. Existing GBA write confirmation behavior is retained.
Automated full-dump RTL simulation, host SHA256/file/error tests and GBA regressions passed. Hardware dump and emulator verification remain pending.
$($timing -join "`r`n")
Mori reference: references/MoriBunrnner-for-ESP32-S3/main/burner/core/burn/burner_cart_common.c and burner_burn_common.c. ESP storage transport is replaced by the verified Pocket output slot/reread path.
Reproduce host tests: bash tests/check-ui106-host.sh <Crystal.gbc> <mengjiandao.gbc>
Reproduce RTL tests: bash tests/check-ui106-dump-rtl.sh
"@,$utf8)
$sources=@($changed)+@(Get-ChildItem app/ui98 -File | ForEach-Object{'app/ui98/'+$_.Name})+@('app/gb_mbc5_fullrom86/sha256.c','app/gb_mbc5_fullrom86/sha256.h','tests/test-ui106-dump.c','tests/test-ui106-dump-rtl.cpp','tests/check-ui106-host.sh','tests/check-ui106-dump-rtl.sh','tests/build-ui106-package.ps1')
[IO.File]::WriteAllLines("$bundle/SOURCES_SHA256.txt",[string[]]@($sources|Sort-Object -Unique|ForEach-Object{"$(Sha "$root/$_")  $_"}),$utf8)
Copy-Item "$fpga/UI98_INPUTS_SHA256.txt" "$bundle/FPGA_INPUTS_SHA256.txt"
$prefix=[IO.Path]::GetFullPath("$bundle/SD_ROOT").TrimEnd('\')+'\'
$sums=@(Get-ChildItem "$bundle/SD_ROOT" -Recurse -File|Sort-Object FullName|ForEach-Object{"$(Sha $_.FullName)  $($_.FullName.Substring($prefix.Length).Replace('\','/'))"})
[IO.File]::WriteAllLines("$bundle/SHA256SUMS.txt",[string[]]$sums,$utf8)
Compress-Archive -Path "$bundle/*" -DestinationPath "$bundle.zip"
$null=Assert-ChisBurnerZipOsIntegrity -ZipPath "$bundle.zip"
Get-FileHash "$bundle.zip" -Algorithm SHA256
