$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
Set-Location $root
$base=Join-Path $root 'build/smoke/ChisBurner-smoke.102-diagnostics'
$bundle=Join-Path $root 'build/smoke/ChisBurner-smoke.105-lv640-readonly'
if((Test-Path $bundle)-or(Test-Path "$bundle.zip")){throw 'Preserve existing package'}
function Sha($p){(Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash.ToLowerInvariant()}
foreach($line in Get-Content "$base/SHA256SUMS.txt"){
 if($line -notmatch '^([0-9a-f]{64})  (.+)$'){throw 'Bad manifest'}
 if((Sha "$base/SD_ROOT/$($Matches[2])") -cne $Matches[1]){throw 'Baseline changed'}
}
$receipt='build/smoke/ChisBurner-smoke.101-transfer-mapper-fix/SOURCES_SHA256.txt'
foreach($line in Get-Content $receipt){
 if($line -notmatch '^([0-9a-f]{64})  (.+)$'){throw 'Bad source receipt'}
 $digest=$Matches[1];$path=$Matches[2]
 if($path.StartsWith('.integration/')-or $path.StartsWith('core/rtl/')-or $path -eq 'app/gba_program65/program65.h'){
  if((Sha "$root/$path") -cne $digest){throw "Protected source changed: $path"}
 }
}
& wsl.exe -d Ubuntu-24.04 -- bash -lc 'cd /mnt/c/ONS_FPGA/work/ChisBurner && bash tests/check-ui98-host.sh "/mnt/c/Users/lin13/Desktop/game/Pokemon - Emerald Version (USA, Europe).gba" && make -C app/ui98 -j4 BUILD_DIR=/mnt/c/ONS_FPGA/work/ChisBurner/build/ui105-analysis-app /mnt/c/ONS_FPGA/work/ChisBurner/build/ui105-analysis-app/app.elf'
if($LASTEXITCODE){throw 'Tests/build failed'}
New-Item -ItemType Directory -Path $bundle | Out-Null
Copy-Item -LiteralPath "$base/SD_ROOT" -Destination "$bundle/SD_ROOT" -Recurse
# Keep the installed loader filenames; the app displays smoke.105.
Copy-Item 'build/ui105-analysis-app/app.elf' "$bundle/SD_ROOT/Assets/chisburner/common/cb102.elf"
New-Item -ItemType Directory -Path "$bundle/SD_ROOT/Saves/chisburner/common" -Force | Out-Null
$utf8=[Text.UTF8Encoding]::new($false)
[IO.File]::WriteAllText("$bundle/SD_ROOT/Saves/chisburner/common/DIRECTORY.txt",'ChisBurner backup output directory. Preserve existing backups.',$utf8)
$core="$bundle/SD_ROOT/Cores/ChisBurner.ChisBurner"
$meta=Get-Content "$core/core.json" -Raw|ConvertFrom-Json
$meta.core.metadata.version='0.1.0-smoke.105-lv640-readonly'
[IO.File]::WriteAllText("$core/core.json",($meta|ConvertTo-Json -Depth 20),$utf8)
[IO.File]::WriteAllText("$core/info.txt",'smoke.105: MX29LV640EB compatible ID and datasheet CFI geometry; read only. GB writes disabled. OS/FPGA unchanged.',$utf8)
foreach($rel in @('Assets/chisburner/common/os.bin','Cores/ChisBurner.ChisBurner/chis.rbf_r')){
 if((Sha "$bundle/SD_ROOT/$rel") -cne (Sha "$base/SD_ROOT/$rel")){throw 'OS/FPGA drift'}
}
[IO.File]::WriteAllText("$bundle/README.txt",@"
Merge SD_ROOT contents into SD root; preserve existing Saves and backups.
App version: smoke.105. Loader filenames and backup prefix remain cb102.
Test Camera: if invalid, open Cart Info and report GB rc, block mask, type, logo, checksum and start bytes. No header checks relaxed.
Test MBC30 LV cart: expect compatible LV model, 8192 KiB, 8 x 8 KiB + 127 x 64 KiB, R1 0x010000, no buffer programming, read-only profile. Writes stay disabled. Repeat analysis twice and check the MBC5 GL cart still reports 32-byte buffer. C2/CB is shared: this does not prove a unique silicon or board model.
Test repeated refresh/analyze with MBC5 and GBA. GBA transfer engines unchanged.
Other GB mappers: header-only result; no NOR commands. No supported NOR does NOT prove retail authenticity.
GB ROM/save writes and GB backup remain unavailable. RTC is declared only, not probed.
LV reference: Macronix PM1328 revision 1.7, tables 4-3/4-4, https://www.macronix.com/Lists/Datasheet/Attachments/8514/MX29LV640E%20T-B%2C%203V%2C%2064Mb%2C%20v1.7.pdf . Tests use synthetic datasheet snapshots; physical CFI remains to be tested. This is NOT a full FlashGBX profile-database port. Unsupported IDs/protocols remain unsupported.
No game ROMs included. Host tests and application build passed; hardware validation pending.
"@,$utf8)
$prefix=[IO.Path]::GetFullPath("$bundle/SD_ROOT").TrimEnd('\')+'\'
$sums=@(Get-ChildItem "$bundle/SD_ROOT" -Recurse -File|Sort-Object FullName|ForEach-Object{"$(Sha $_.FullName)  $($_.FullName.Substring($prefix.Length).Replace('\','/'))"})
[IO.File]::WriteAllLines("$bundle/SHA256SUMS.txt",[string[]]$sums,$utf8)
Compress-Archive -Path "$bundle/*" -DestinationPath "$bundle.zip"
Get-FileHash "$bundle.zip" -Algorithm SHA256


