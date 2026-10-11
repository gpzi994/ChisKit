$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$dst="$root/build/sdram126-noflush-seed6/quartus"
$mif="$root/.integration/openfpgaos/src/firmware/os/bld/sdram126-boot/firmware.mif"
$quartus='C:/altera_lite/25.1std/quartus/bin64'
if(!(Test-Path -LiteralPath $mif)){throw 'Build SDRAM126 OS first'}
Copy-Item -LiteralPath $mif -Destination "$dst/firmware.mif" -Force
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
