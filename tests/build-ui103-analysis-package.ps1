$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
Set-Location $root
$base=Join-Path $root 'build/smoke/ChisBurner-smoke.102-diagnostics'
$bundle=Join-Path $root 'build/smoke/ChisBurner-smoke.103-staged-analysis'
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
& wsl.exe -d Ubuntu-24.04 -- bash -lc 'cd /mnt/c/ONS_FPGA/work/ChisBurner && bash tests/check-ui98-host.sh "/mnt/c/Users/lin13/Desktop/game/Pokemon - Emerald Version (USA, Europe).gba" && make -C app/ui98 -j4 BUILD_DIR=/mnt/c/ONS_FPGA/work/ChisBurner/build/ui103-analysis-app /mnt/c/ONS_FPGA/work/ChisBurner/build/ui103-analysis-app/app.elf'
if($LASTEXITCODE){throw 'Tests/build failed'}
New-Item -ItemType Directory -Path $bundle | Out-Null
Copy-Item -LiteralPath "$base/SD_ROOT" -Destination "$bundle/SD_ROOT" -Recurse
# Keep the installed loader filenames; the app displays smoke.103.
Copy-Item 'build/ui103-analysis-app/app.elf' "$bundle/SD_ROOT/Assets/chisburner/common/cb102.elf"
New-Item -ItemType Directory -Path "$bundle/SD_ROOT/Saves/chisburner/common" -Force | Out-Null
$utf8=[Text.UTF8Encoding]::new($false)
[IO.File]::WriteAllText("$bundle/SD_ROOT/Saves/chisburner/common/DIRECTORY.txt",'ChisBurner backup output directory. Preserve existing backups.',$utf8)
$core="$bundle/SD_ROOT/Cores/ChisBurner.ChisBurner"
$meta=Get-Content "$core/core.json" -Raw|ConvertFrom-Json
$meta.core.metadata.version='0.1.0-smoke.103-staged-analysis'
[IO.File]::WriteAllText("$core/core.json",($meta|ConvertTo-Json -Depth 20),$utf8)
[IO.File]::WriteAllText("$core/info.txt",'smoke.103: MBC30 header classification, mapper warning separated from supported NOR query. GB writes disabled. OS/FPGA unchanged.',$utf8)
foreach($rel in @('Assets/chisburner/common/os.bin','Cores/ChisBurner.ChisBurner/chis.rbf_r')){
 if((Sha "$bundle/SD_ROOT/$rel") -cne (Sha "$base/SD_ROOT/$rel")){throw 'OS/FPGA drift'}
}
[IO.File]::WriteAllText("$bundle/README.txt",@"
Merge SD_ROOT contents into SD root; preserve existing Saves and backups.
App version: smoke.103. Loader filenames and backup prefix remain cb102.
Test MBC3/MBC30: bank0 equality now warns, then queries the existing NOR profile.
Report NOR model, JEDEC, geometry, status and rc. A mapper warning is NOT hardware validation.
Test repeated refresh/analyze with MBC5 and GBA. GBA transfer engines unchanged.
Other GB mappers: header-only result; no NOR commands. No supported NOR does NOT prove retail authenticity.
GB ROM/save writes and GB backup remain unavailable. RTC is declared only, not probed.
This is NOT a full FlashGBX profile-database port. Unsupported IDs/protocols remain unsupported.
No game ROMs included. Host tests and application build passed; hardware validation pending.
"@,$utf8)
$prefix=[IO.Path]::GetFullPath("$bundle/SD_ROOT").TrimEnd('\')+'\'
$sums=@(Get-ChildItem "$bundle/SD_ROOT" -Recurse -File|Sort-Object FullName|ForEach-Object{"$(Sha $_.FullName)  $($_.FullName.Substring($prefix.Length).Replace('\','/'))"})
[IO.File]::WriteAllLines("$bundle/SHA256SUMS.txt",[string[]]$sums,$utf8)
Compress-Archive -Path "$bundle/*" -DestinationPath "$bundle.zip"
Get-FileHash "$bundle.zip" -Algorithm SHA256
