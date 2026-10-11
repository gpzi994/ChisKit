param([switch]$Resume,[ValidateSet('stream131-r8-quartus','stream131-r8-timing2-quartus','stream131-r8-timing3-quartus','stream131-r8-timing4-quartus')][string]$BuildName='stream131-r8-quartus',[ValidateRange(1,99)][int]$Seed=6)
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$src="$root/build/stream131-r6-quartus"
$dst="$root/build/$BuildName"
$mif="$root/.integration/openfpgaos/src/firmware/os/bld/stream131-r8-release/firmware.mif"
$quartus='C:/altera_lite/25.1std/quartus/bin64'
if(!(Test-Path -LiteralPath $mif)){throw 'Build r8 OS first'}
if(Test-Path -LiteralPath $dst){if(!$Resume){throw "Preserve existing build: $dst"}}
else {Copy-Item -LiteralPath $src -Destination $dst -Recurse}
Copy-Item -LiteralPath $mif -Destination "$dst/firmware.mif" -Force
Push-Location $dst
try {
 $ErrorActionPreference='Continue' # Quartus writes harmless TBB diagnostics to stderr.
 & "$quartus/quartus_sh.exe" -t "$PSScriptRoot/configure-stream131.tcl" $dst "$dst/firmware.mif" $Seed *> r8-configure.log
 if($LASTEXITCODE){throw 'r8 configure failed'}
 & "$quartus/quartus_sh.exe" --flow compile ap_core *> r8-full-compile.log
 if($LASTEXITCODE){throw 'r8 full FPGA compilation failed'}
 & "$quartus/quartus_sta.exe" -t "$PSScriptRoot/check-sdram124-sta.tcl" $dst *> r8-cdc-report.log
 if($LASTEXITCODE){throw 'r8 CDC reports failed'}
} finally {$ErrorActionPreference='Stop';Pop-Location}
& "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $dst
