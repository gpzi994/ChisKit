$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
Set-Location $root
$base=Join-Path $root 'build/smoke/ChisBurner-smoke.101-transfer-mapper-fix'
$bundle=Join-Path $root 'build/smoke/ChisBurner-smoke.102-diagnostics'
if((Test-Path $bundle)-or(Test-Path "$bundle.zip")){throw 'Preserve existing smoke.102 package'}
function Sha($p){(Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash.ToLowerInvariant()}
foreach($line in Get-Content "$base/SHA256SUMS.txt"){
 if($line -notmatch '^([0-9a-f]{64})  (.+)$'){throw 'Malformed baseline manifest'}
 if((Sha "$base/SD_ROOT/$($Matches[2])") -cne $Matches[1]){throw 'Baseline drift'}
}
# Reusing existing hardware is allowed only while engine and OS sources agree
# with the source receipt of the accepted package. App source changes are intended.
foreach($line in Get-Content "$base/SOURCES_SHA256.txt"){
 if($line -notmatch '^([0-9a-f]{64})  (.+)$'){throw 'Malformed source receipt'}
 $digest=$Matches[1];$path=$Matches[2]
 if($path.StartsWith('.integration/')-or $path.StartsWith('core/rtl/')-or $path -eq 'app/gba_program65/program65.h'){
  if((Sha "$root/$path") -cne $digest){throw "Protected source changed: $path"}
 }
}
& wsl.exe -d Ubuntu-24.04 -- bash /mnt/c/ONS_FPGA/work/ChisBurner/tests/check-ui102.sh
if($LASTEXITCODE){throw 'Host tests or application build failed'}
$utf8=[Text.UTF8Encoding]::new($false)
New-Item -ItemType Directory -Path $bundle | Out-Null
Copy-Item -LiteralPath "$base/SD_ROOT" -Destination "$bundle/SD_ROOT" -Recurse
$common="$bundle/SD_ROOT/Assets/chisburner/common"
$core="$bundle/SD_ROOT/Cores/ChisBurner.ChisBurner"
Copy-Item 'build/ui102-app/app.elf' "$common/cb102.elf"
[IO.File]::WriteAllText("$common/cb102.ini","[os]`nELF=cb102.elf`nARGS=`n",$utf8)
$data=Get-Content "$core/data.json" -Raw|ConvertFrom-Json
($data.data.data_slots|Where-Object id -eq 2).filename='cb102.ini'
($data.data.data_slots|Where-Object id -eq 3).filename='cb102.elf'
[IO.File]::WriteAllText("$core/data.json",($data|ConvertTo-Json -Depth 20),$utf8)
$meta=Get-Content "$core/core.json" -Raw|ConvertFrom-Json
$meta.core.metadata.version='0.1.0-smoke.102-diagnostics'
$meta.core.metadata.description='Mapper failure stages and backup file-end / write diagnostics'
[IO.File]::WriteAllText("$core/core.json",($meta|ConvertTo-Json -Depth 20),$utf8)
[IO.File]::WriteAllText("$core/info.txt",'smoke.102 diagnostic app. OS and FPGA identical to smoke.101. Hardware fault resolution pending. No changes to GBA erase/program or GB write permissions.',$utf8)
Copy-Item 'app/ui98/README_SMOKE102_CN.md' "$bundle/README_TEST_CN.md"
Copy-Item 'build/ui102-app-sources.sha256' "$bundle/APP_SOURCES_SHA256.txt"
foreach($rel in @('Assets/chisburner/common/os.bin','Cores/ChisBurner.ChisBurner/chis.rbf_r')){
 if((Sha "$bundle/SD_ROOT/$rel") -cne (Sha "$base/SD_ROOT/$rel")){throw 'OS/FPGA reuse failed'}
}
[IO.File]::WriteAllLines("$bundle/BUILD_PROVENANCE.txt",[string[]]@(
 'release=smoke.102-diagnostics','host_asan_ubsan=PASS','riscv_application_build=PASS',
 'os_and_fpga=byte_identical_to_smoke.101','fpga_recompile=NO','hardware_test=PENDING',
 'fault_fixed=NOT_CLAIMED','gba_program_engine=UNCHANGED','gb_save_rtc_writes=DISABLED',
 "app_sha256=$(Sha "$common/cb102.elf")","os_sha256=$(Sha "$common/os.bin")",
 "rbf_r_sha256=$(Sha "$core/chis.rbf_r")"
),$utf8)
$prefix=[IO.Path]::GetFullPath("$bundle/SD_ROOT").TrimEnd('\')+'\'
$sums=@(Get-ChildItem "$bundle/SD_ROOT" -Recurse -File|Sort-Object FullName|ForEach-Object{
 "$(Sha $_.FullName)  $($_.FullName.Substring($prefix.Length).Replace('\','/'))"
})
[IO.File]::WriteAllLines("$bundle/SHA256SUMS.txt",[string[]]$sums,$utf8)
Compress-Archive -Path "$bundle/*" -DestinationPath "$bundle.zip" -CompressionLevel Optimal
Get-FileHash "$bundle.zip" -Algorithm SHA256
