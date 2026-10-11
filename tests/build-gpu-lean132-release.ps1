$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$src="$root/build/gpu-lean132-quartus"
$dst="$root/build/gpu-lean132-release-quartus"
$os="$root/.integration/openfpgaos/src/firmware/os/bld/gpu-lean132-release"
$q='C:/altera_lite/25.1std/quartus/bin64'
if(Test-Path -LiteralPath $dst){throw 'Preserve existing release build'}
if(!(Test-Path -LiteralPath "$os/firmware.mif")){throw 'Missing release MIF'}
$null=New-Item -ItemType Directory -Path $dst
foreach($name in @('ap_core.qpf','ap_core.qsf')){Copy-Item -LiteralPath "$src/$name" -Destination "$dst/$name"}
Copy-Item -LiteralPath "$os/firmware.mif" -Destination "$dst/firmware.mif"
Push-Location $dst
try {
 $ErrorActionPreference='Continue'
 & "$q/quartus_sh.exe" -t "$PSScriptRoot/configure-gpu-lean132.tcl" $dst *> release-configure.log
 if($LASTEXITCODE){throw 'Release configure failed'}
 & "$q/quartus_sh.exe" --flow compile ap_core *> release-full-compile.log
 if($LASTEXITCODE){throw 'Full compile failed; do not package'}
 & "$q/quartus_sta.exe" -t "$PSScriptRoot/check-sdram124-sta.tcl" $dst *> release-cdc-report.log
 if($LASTEXITCODE){throw 'CDC reporting failed; do not package'}
} finally {$ErrorActionPreference='Stop';Pop-Location}
& "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $dst
