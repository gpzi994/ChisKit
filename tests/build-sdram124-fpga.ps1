$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$dst=Join-Path $root 'build/sdram124/quartus'
$quartus='C:/altera_lite/25.1std/quartus/bin64'
& "$quartus/quartus_sh.exe" -t "$PSScriptRoot/check-sdram124-fit.tcl" *> "$root/build/sdram124/full-fit.log"
if($LASTEXITCODE){throw 'SDRAM124 synthesis/fit/timing failed; preserve logs'}
Push-Location $dst
try {
 & "$quartus/quartus_cdb.exe" ap_core --update_mif *> update-mif.log
 if($LASTEXITCODE){throw 'MIF update failed'}
 & "$quartus/quartus_asm.exe" --read_settings_files=on --write_settings_files=off ap_core *> assemble.log
 if($LASTEXITCODE){throw 'Assembler failed'}
 & "$quartus/quartus_sta.exe" -t "$PSScriptRoot/check-sdram124-sta.tcl" $dst *> cdc-report.log
 if($LASTEXITCODE){throw 'CDC reports failed'}
} finally {Pop-Location}
& "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $dst
