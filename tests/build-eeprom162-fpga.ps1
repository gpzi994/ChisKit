param([switch]$Resume)
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$src="$root/build/save160-r5-quartus"
$dst="$root/build/eeprom162-quartus"
$mif="$root/.integration/openfpgaos/src/firmware/os/bld/eeprom162/firmware.mif"
$quartus='C:/altera_lite/25.1std/quartus/bin64'
if(!(Test-Path -LiteralPath $mif)){throw 'Build EEPROM162 OS first'}
if(!(Test-Path -LiteralPath "$src/output_files/ap_core.sta.summary")){throw 'R5 base missing'}
if(Test-Path -LiteralPath $dst){if(!$Resume){throw "Preserve existing build: $dst"}}
else {
 $null=New-Item -ItemType Directory -Path $dst
 foreach($name in @('ap_core.qpf','ap_core.qsf')){
  Copy-Item -LiteralPath "$src/$name" -Destination "$dst/$name"
 }
}
Copy-Item -LiteralPath $mif -Destination "$dst/firmware.mif" -Force
Push-Location $dst
try {
 $ErrorActionPreference='Continue'
 & "$quartus/quartus_sh.exe" -t "$PSScriptRoot/configure-save160.tcl" $dst *> configure.log
 if($LASTEXITCODE){throw 'EEPROM162 configure failed'}
 & "$quartus/quartus_sh.exe" --flow compile ap_core *> full-compile.log
 if($LASTEXITCODE){throw 'EEPROM162 full FPGA compilation failed'}
 & "$quartus/quartus_sta.exe" -t "$PSScriptRoot/check-sdram124-sta.tcl" $dst *> cdc-report.log
 if($LASTEXITCODE){throw 'EEPROM162 timing/CDC report generation failed'}
} finally {$ErrorActionPreference='Stop';Pop-Location}
& "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $dst -AllowUnusedBridgeReady
Get-Content "$dst/output_files/ap_core.fit.summary"
Get-Content "$dst/output_files/ap_core.sta.summary"
