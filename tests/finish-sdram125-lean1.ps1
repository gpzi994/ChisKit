$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$dst="$root/build/sdram125-lean1/quartus"
$quartus='C:/altera_lite/25.1std/quartus/bin64'
& "$quartus/quartus_sh.exe" -t "$PSScriptRoot/build-sdram125-lean1.tcl" *> "$root/build/sdram125-lean1/build.log"
if($LASTEXITCODE){throw 'Map/fit/STA failed'}
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
