$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$src=Join-Path $root 'build/sdram121/quartus'
$dst=Join-Path $root 'build/sdram123-r3/quartus'
$os=Join-Path $root '.integration/openfpgaos/src/firmware/os/bld/sdram123-r3-boot'
$quartus='C:/altera_lite/25.1std/quartus/bin64'
if(Test-Path -LiteralPath $dst){throw 'Preserve existing SDRAM123 FPGA build; inspect before retry.'}
& "$PSScriptRoot/check-sdram121-reports.ps1" -reportRoot $src
if(-not(Test-Path "$os/firmware.mif")){throw 'Build diagnostic OS and firmware.mif first'}
Copy-Item -LiteralPath $src -Destination $dst -Recurse
Copy-Item -LiteralPath "$os/firmware.mif" -Destination "$dst/firmware.mif" -Force
Push-Location $dst
try {
 & "$quartus/quartus_cdb.exe" ap_core --update_mif *> update-mif.log
 if($LASTEXITCODE){throw 'Quartus memory initialization update failed'}
 & "$quartus/quartus_asm.exe" --read_settings_files=on --write_settings_files=off ap_core *> assemble.log
 if($LASTEXITCODE){throw 'Quartus assembler failed'}
 & "$quartus/quartus_sta.exe" ap_core *> sta.log
 if($LASTEXITCODE){throw 'Quartus timing analysis failed'}
 & "$quartus/quartus_sta.exe" -t "$PSScriptRoot/check-sdram121-sta.tcl" $dst *> cdc-report.log
 if($LASTEXITCODE){throw 'Quartus CDC reports failed'}
} finally {Pop-Location}
& "$PSScriptRoot/check-sdram121-reports.ps1" -reportRoot $dst
Get-FileHash "$dst/firmware.mif","$dst/output_files/ap_core.rbf" -Algorithm SHA256
