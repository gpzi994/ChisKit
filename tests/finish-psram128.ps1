$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$src="$root/build/psram128-r6-seed4/quartus"
$dst="$root/build/psram128-r7-seed2/quartus"
$mif="$root/.integration/openfpgaos/src/firmware/os/bld/psram128/firmware.mif"
$quartus='C:/altera_lite/25.1std/quartus/bin64'
if(!(Test-Path -LiteralPath $mif)){throw 'Build PSRAM128 OS first'}
if(Test-Path -LiteralPath $dst){throw 'Preserve existing PSRAM128-r6 Quartus build'}
& "$PSScriptRoot/check-psram128-getfile.ps1"
& "$PSScriptRoot/check-psram128-flush-admission.ps1"
Copy-Item -LiteralPath $src -Destination $dst -Recurse
Copy-Item -LiteralPath $mif -Destination "$dst/firmware.mif" -Force
Push-Location $dst
try {
 & "$quartus/quartus_sh.exe" -t "$PSScriptRoot/configure-psram128-seed.tcl" $dst 2 *> configure-seed.log
 if($LASTEXITCODE){throw 'Seed configuration failed'}
 & "$quartus/quartus_sh.exe" --flow compile ap_core *> full-compile.log
 if($LASTEXITCODE){throw 'Full FPGA compile failed'}
 Copy-Item -LiteralPath full-compile.log -Destination assemble.log -Force
 & "$quartus/quartus_sta.exe" -t "$PSScriptRoot/check-sdram124-sta.tcl" $dst *> cdc-report.log
 if($LASTEXITCODE){throw 'CDC reports failed'}
} finally {Pop-Location}
& "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $dst
