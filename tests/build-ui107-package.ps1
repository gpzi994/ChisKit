param(
 [int]$Seed=4,
 [ValidateSet('standard','closure','floorplan','floorplan5','floorplan5control','phys','retime','control')][string]$Variant='floorplan5control'
)
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
Set-Location $root
function Sha($p){(Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash.ToLowerInvariant()}
$base=Join-Path $root 'build/smoke/ChisBurner-smoke.106-gb-dump-r1'
$bundle=Join-Path $root 'build/smoke/ChisBurner-smoke.107-mapper-dump-r1'
$suffix=if($Variant -eq 'standard'){''}else{"-$Variant"}
$fpga=Join-Path $root ".integration/openfpgaos/src/fpga/targets/pocket/bld/chisburner-ui107-s$Seed$suffix-win"
$os=Join-Path $root '.integration/openfpgaos/src/firmware/os/bld/ui107-pocket'
if((Test-Path $bundle)-or(Test-Path "$bundle.zip")){throw 'Preserve existing package'}
foreach($line in Get-Content 'build/ui107-app/sources.sha256'){
 if($line -notmatch '^([0-9a-f]{64})  (.+)$'){throw 'Bad application build receipt'}
 $digest=$Matches[1];$path=$Matches[2]
 if($path.StartsWith('/mnt/c/')){$path='C:/'+$path.Substring(7)}
 elseif(-not[IO.Path]::IsPathRooted($path)){$path=Join-Path $root $path}
 if((Sha $path) -cne $digest){throw "Application build input changed: $path"}
}
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
& "$PSScriptRoot/check-ui107-fpga-inputs.ps1" -Seed $Seed -Variant $Variant
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
if(-not('ChisBurner.Ui107.Reverse'-as[type])){Add-Type -TypeDefinition @'
namespace ChisBurner.Ui107 { public static class Reverse { public static byte[] Apply(byte[] i) { var t=new byte[256]; for(int n=0;n<256;n++){int x=n,y=0;for(int b=0;b<8;b++){y=(y<<1)|(x&1);x>>=1;}t[n]=(byte)y;}var o=new byte[i.Length];for(int n=0;n<i.Length;n++)o[n]=t[i[n]];return o; } } }
'@}
New-Item -ItemType Directory -Path $bundle | Out-Null
Copy-Item -LiteralPath "$base/SD_ROOT" -Destination "$bundle/SD_ROOT" -Recurse
Copy-Item 'build/ui107-app/app.elf' "$bundle/SD_ROOT/Assets/chisburner/common/cb102.elf"
Copy-Item "$os/os.bin" "$bundle/SD_ROOT/Assets/chisburner/common/os.bin"
$core="$bundle/SD_ROOT/Cores/ChisBurner.ChisBurner"
[IO.File]::WriteAllBytes("$core/chis.rbf_r",[ChisBurner.Ui107.Reverse]::Apply([IO.File]::ReadAllBytes("$fpga/output_files/ap_core.rbf")))
$utf8=[Text.UTF8Encoding]::new($false)
$meta=Get-Content "$core/core.json" -Raw|ConvertFrom-Json
$meta.core.metadata.version='0.1.0-smoke.107-mapper-dump'
[IO.File]::WriteAllText("$core/core.json",($meta|ConvertTo-Json -Depth 20),$utf8)
$data=Get-Content "$core/data.json" -Raw|ConvertFrom-Json
$outputSlot=@($data.data.data_slots|Where-Object id -eq 5)
if($outputSlot.Count -ne 1){throw 'Missing unique output slot'}
$outputSlot[0].extensions=@('gba','gb','gbc')
[IO.File]::WriteAllText("$core/data.json",($data|ConvertTo-Json -Depth 20),$utf8)
[IO.File]::WriteAllText("$core/info.txt",'smoke.107: header-sized retail/flashcart GB ROM dump; mirror-estimated GBA ROM dump. New OS and FPGA required. GB ROM/save writes disabled.',$utf8)
New-Item -ItemType Directory -Path "$bundle/SD_ROOT/Saves/chisburner/common" -Force | Out-Null
[IO.File]::WriteAllText("$bundle/README.txt",@"
ChisBurner smoke.107 - cartridge ROM size policy and mapper-specific dumps
Install ALL SD_ROOT contents into the SD root, including os.bin and chis.rbf_r. Do not update only the app. Preserve existing Saves and backups. Loader filename cb102.elf is intentional.
GBA: estimate power-of-two mirroring using repeated read-only samples; no ROM-size field exists in its header. No mirror => 32 MiB address window. This is not the original software file length. An 8 MiB image on non-mirroring 32 MiB NOR still dumps 32 MiB. Never trim FF tails. NOR geometry remains separate from ROM dump size.
GB/GBC: use valid header ROM capacity, constrained by mapper support. NOR identification is NOT required for reading retail or flash cartridges.
Read-only mapper limits: ROM-only 32 KiB; standard MBC1 2 MiB; MBC2 256 KiB; MBC3 2 MiB; MBC30 4 MiB; MBC5 8 MiB; MBC7 4 MiB; Camera/MAC-GBD 1 MiB.
Known MBC1 multicart titles, unsupported mappers, invalid/unstable headers and out-of-range sizes remain blocked. Camera support here is its ROM-bank read path only: the previously observed zero-header issue is NOT fixed or bypassed. No camera-register, SRAM, EEPROM or RTC access occurs.
For each card: Refresh, select Backup ROM, release A and press A again to confirm. No source-file selection is needed. NOR warnings do not authorize writes and do not block a valid supported ROM dump.
GB files: Saves/chisburner/common/cb107-{rom|crystal|zelda}-*.gbc. GBA files retain cb102-*.gba. All GB outputs use .gbc; size/bytes are unchanged.
Every GB dump is reread from SD and SHA256-checked against the cartridge read stream. Generic ROM success says "File verified; no reference ROM": this proves file/stream consistency, not independent source authenticity. Compare repeated dumps and test in an emulator.
The two previously supplied matching headers additionally retain strict full-reference SHA256 checks:
Crystal: 136ada06cb68656b7de475fa4b278d37dbeff8f5257e7dfdf7f4a4aec19a90f3
Zelda: 8ba533be5908305a0f2f04117a858a91d4098ed8d4d2be6e48c3c3e9df1885e7
No game ROM data is bundled. Modified ROMs with one of those same headers may fail the strict reference check; keep/report the file rather than treating it as an accepted reference.
GBA output retains full reread MD5 + CRC32 verification. Partial/error files are retained, never accepted as successful backups.
Only mapper ROM-bank register writes occur during GB dumps. No GB NOR erase/program, SRAM or RTC writes. GBA burner engine/program sources are protected unchanged; router/OS/FPGA updated.
Hardware acceptance: two dumps each of Tetris DX (MBC1), Pokemon Gold (MBC3), Kirby Tilt 'n' Tumble (MBC7), and regression MBC30 Crystal/MBC5 Zelda; compare lengths/hashes and run in emulator. Recheck GBA retail + NOR backups and refresh/analyze. Camera is testable only after a valid header is obtained. Synthetic full-address-range RTL tests do not replace hardware verification.
$($timing -join "`r`n")
Reference flow: C:/ONS_FPGA/references/MoriBunrnner-for-ESP32-S3/main/burner/core/burn/burner_burn_common.c (64 KiB storage chunks, banked ROM reads). Pocket uses 32-byte cartridge transactions and verified output-slot reread.
Host tests: bash tests/check-ui107-host.sh <Crystal.gbc> <mengjiandao.gbc>
GBA/UI regression: bash tests/check-ui98-host.sh <GBA.gba>
RTL tests: bash tests/check-ui107-dump-rtl.sh; bash tests/check-ui106-dump-rtl.sh; bash tests/check-ui98-rtl.sh
"@,$utf8)
$sources=@($changed)+@(Get-ChildItem app/ui98 -File | ForEach-Object{'app/ui98/'+$_.Name})+@('app/gb_mbc5_fullrom86/sha256.c','app/gb_mbc5_fullrom86/sha256.h','tests/test-ui107-size.c','tests/test-ui107-gba8.c','tests/test-ui98-transfer.c','tests/test-ui98.c','tests/test-ui107-dump.c','tests/test-ui107-dump-rtl.cpp','tests/check-ui107-host.sh','tests/check-ui107-dump-rtl.sh','tests/build-ui107-package.ps1')
[IO.File]::WriteAllLines("$bundle/SOURCES_SHA256.txt",[string[]]@($sources|Sort-Object -Unique|ForEach-Object{"$(Sha "$root/$_")  $_"}),$utf8)
Copy-Item "$fpga/UI98_INPUTS_SHA256.txt" "$bundle/FPGA_INPUTS_SHA256.txt"
Copy-Item 'build/ui107-app/sources.sha256' "$bundle/APP_INPUTS_SHA256.txt"
Copy-Item 'docs/SMOKE107_DUMP_CN.md' "$bundle/TEST_CN.md"
Copy-Item 'docs/SMOKE107_VERIFICATION.md' "$bundle/VERIFICATION.md"
$prefix=[IO.Path]::GetFullPath("$bundle/SD_ROOT").TrimEnd('\')+'\'
$sums=@(Get-ChildItem "$bundle/SD_ROOT" -Recurse -File|Sort-Object FullName|ForEach-Object{"$(Sha $_.FullName)  $($_.FullName.Substring($prefix.Length).Replace('\','/'))"})
[IO.File]::WriteAllLines("$bundle/SHA256SUMS.txt",[string[]]$sums,$utf8)
Compress-Archive -Path "$bundle/*" -DestinationPath "$bundle.zip"
$null=Assert-ChisBurnerZipOsIntegrity -ZipPath "$bundle.zip"
Get-FileHash "$bundle.zip" -Algorithm SHA256
