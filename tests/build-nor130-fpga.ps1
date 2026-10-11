param([int]$Seed=2)
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$src="$root/build/psram129-r1-seed2/quartus"
$dst="$root/build/nor130-r1-seed$Seed/quartus"
$mif="$root/.integration/openfpgaos/src/firmware/os/bld/nor130/firmware.mif"
$quartus='C:/altera_lite/25.1std/quartus/bin64'
if(!(Test-Path -LiteralPath $mif)){throw 'Build NOR130 OS first'}
if(Test-Path -LiteralPath $dst){throw "Preserve existing build: $dst"}
Copy-Item -LiteralPath $src -Destination $dst -Recurse
Copy-Item -LiteralPath $mif -Destination "$dst/firmware.mif" -Force
Push-Location $dst
try {
 & "$quartus/quartus_sh.exe" -t "$PSScriptRoot/configure-nor130.tcl" $dst "$dst/firmware.mif" $Seed *> configure.log
 if($LASTEXITCODE){throw 'NOR130 Quartus configuration failed'}
 & "$quartus/quartus_sh.exe" --flow compile ap_core *> full-compile.log
 if($LASTEXITCODE){throw 'NOR130 full FPGA compile failed'}
 & "$quartus/quartus_sta.exe" -t "$PSScriptRoot/check-sdram124-sta.tcl" $dst *> cdc-report.log
 if($LASTEXITCODE){throw 'NOR130 CDC report generation failed'}
} finally { Pop-Location }
& "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $dst
Write-Output "NOR130 FPGA seed $Seed complete: $dst"
