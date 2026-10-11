param([ValidateRange(2,100)][int]$Seed=2)
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$src="$root/build/eeprom162-quartus"
$dst="$root/build/eeprom162-seed$Seed-quartus"
$mif="$root/.integration/openfpgaos/src/firmware/os/bld/eeprom162/firmware.mif"
$quartus='C:/altera_lite/25.1std/quartus/bin64'
if(!(Test-Path -LiteralPath "$src/output_files/ap_core.rbf")){throw 'Baseline FPGA build missing'}
if(Test-Path -LiteralPath $dst){throw "Preserve existing build: $dst"}
$null=New-Item -ItemType Directory -Path $dst
foreach($name in @('ap_core.qpf','ap_core.qsf')){
 Copy-Item -LiteralPath "$src/$name" -Destination "$dst/$name"
}
Copy-Item -LiteralPath $mif -Destination "$dst/firmware.mif"
Push-Location $dst
try {
 $ErrorActionPreference='Continue'
 & "$quartus/quartus_sh.exe" -t "$PSScriptRoot/configure-eeprom162-seed.tcl" $dst $Seed *> configure.log
 if($LASTEXITCODE){throw 'EEPROM162 seed configure failed'}
 & "$quartus/quartus_sh.exe" --flow compile ap_core *> full-compile.log
 if($LASTEXITCODE){throw 'EEPROM162 seed full compilation failed'}
 & "$quartus/quartus_sta.exe" -t "$PSScriptRoot/check-sdram124-sta.tcl" $dst *> cdc-report.log
 if($LASTEXITCODE){throw 'EEPROM162 seed timing/CDC report failed'}
} finally {$ErrorActionPreference='Stop';Pop-Location}
& "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $dst -AllowUnusedBridgeReady
Get-Content "$dst/output_files/ap_core.fit.summary"
Get-Content "$dst/output_files/ap_core.sta.summary"
