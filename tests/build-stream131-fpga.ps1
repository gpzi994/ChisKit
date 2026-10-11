param([int]$Seed=6)
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$src="$root/build/nor130-r1-seed6/quartus"
$dst="$root/build/stream131-r1-seed$Seed/quartus"
$mif="$root/.integration/openfpgaos/src/firmware/os/bld/stream131-release/firmware.mif"
$quartus='C:/altera_lite/25.1std/quartus/bin64'
if(!(Test-Path -LiteralPath $mif)){throw 'Build Stream131 release OS first'}
if(Test-Path -LiteralPath $dst){throw "Preserve existing build: $dst"}
Copy-Item -LiteralPath $src -Destination $dst -Recurse
Copy-Item -LiteralPath $mif -Destination "$dst/firmware.mif" -Force
Push-Location $dst
try {
 & "$quartus/quartus_sh.exe" -t "$PSScriptRoot/configure-stream131.tcl" $dst "$dst/firmware.mif" $Seed *> configure.log
 if($LASTEXITCODE){throw 'Stream131 Quartus configuration failed'}
 & "$quartus/quartus_sh.exe" --flow compile ap_core *> full-compile.log
 if($LASTEXITCODE){throw 'Stream131 full FPGA compile failed'}
 & "$quartus/quartus_sta.exe" -t "$PSScriptRoot/check-sdram124-sta.tcl" $dst *> cdc-report.log
 if($LASTEXITCODE){throw 'Stream131 CDC report generation failed'}
} finally { Pop-Location }
& "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $dst
Write-Output "Stream131 FPGA seed $Seed complete: $dst"
