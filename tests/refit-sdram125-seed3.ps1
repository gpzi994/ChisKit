$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$src="$root/build/sdram125-cpu1/quartus"
$parent="$root/build/sdram125-seed3"
$dst="$parent/quartus"
$quartus='C:/altera_lite/25.1std/quartus/bin64'
if(Test-Path $parent){throw 'Preserve existing seed3 attempt'}
New-Item -ItemType Directory $parent | Out-Null
Copy-Item -LiteralPath $src -Destination $dst -Recurse
& "$quartus/quartus_sh.exe" -t "$PSScriptRoot/refit-sdram125-seed3.tcl" *> "$parent/refit.log"
if($LASTEXITCODE){throw 'Refit failed'}
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
