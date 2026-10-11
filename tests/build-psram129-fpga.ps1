param([int]$Seed=2)
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$src="$root/build/psram128-r7-seed2/quartus"
$dst="$root/build/psram129-r1-seed$Seed/quartus"
$mif="$root/.integration/openfpgaos/src/firmware/os/bld/psram129/firmware.mif"
$quartus='C:/altera_lite/25.1std/quartus/bin64'
if(!(Test-Path -LiteralPath $mif)){throw 'Build PSRAM129 OS first'}
if(Test-Path -LiteralPath $dst){throw "Preserve existing build: $dst"}
Copy-Item -LiteralPath $src -Destination $dst -Recurse
Copy-Item -LiteralPath $mif -Destination "$dst/firmware.mif" -Force
Push-Location $dst
try {
 & "$quartus/quartus_sh.exe" -t "$PSScriptRoot/configure-psram129.tcl" $dst $Seed *> configure.log
 if($LASTEXITCODE){throw 'PSRAM129 Quartus configuration failed'}
 & "$quartus/quartus_sh.exe" --flow compile ap_core *> full-compile.log
 if($LASTEXITCODE){throw 'PSRAM129 full FPGA compile failed'}
 & "$quartus/quartus_sta.exe" -t "$PSScriptRoot/check-sdram124-sta.tcl" $dst *> cdc-report.log
 if($LASTEXITCODE){throw 'PSRAM129 CDC report generation failed'}
} finally { Pop-Location }
& "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $dst
Write-Output "PSRAM129 FPGA seed $Seed complete: $dst"
