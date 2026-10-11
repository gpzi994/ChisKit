$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$src="$root/build/rtc172-quartus"
$dst="$root/build/gb-save177-quartus"
$os="$root/.integration/openfpgaos/src/firmware/os/bld/gb-save177"
$q='C:/altera_lite/25.1std/quartus/bin64'
if(Test-Path -LiteralPath $dst){throw 'Preserve existing experiment'}
& "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $src -AllowUnusedBridgeReady
New-Item -ItemType Directory -Path $dst | Out-Null
# Copy project inputs only; never inherit old implementation reports or database.
Get-ChildItem -LiteralPath $src -File | Where-Object {$_.Extension -in @('.qsf','.qpf','.sdc','.mif','.hex','.v','.sv','.vhd','.qip','.sip','.ip','.tcl')} | ForEach-Object {
 Copy-Item -LiteralPath $_.FullName -Destination $dst
}
Copy-Item -LiteralPath "$os/firmware.mif" -Destination "$dst/firmware.mif" -Force
# Relocate project-generated paths and enable only the new GB RAM aperture.
$qsf=Get-Content -LiteralPath "$dst/ap_core.qsf" -Raw
$qsf=$qsf.Replace($src.Replace('\','/'),$dst.Replace('\','/'))
$qsf+="`nset_global_assignment -name VERILOG_MACRO CHISBURNER_GB_SAVE175`nset_global_assignment -name VERILOG_MACRO CHISBURNER_GB_SAVE177`n"
[IO.File]::WriteAllText("$dst/ap_core.qsf",$qsf,[Text.UTF8Encoding]::new($false))
Push-Location $dst
try {
 $ErrorActionPreference='Continue'
 & "$q/quartus_sh.exe" --flow compile ap_core *> compile177.log
 if($LASTEXITCODE){throw 'Full compile failed; inspect compile177.log'}
 & "$q/quartus_sta.exe" -t "$PSScriptRoot/check-sdram124-sta.tcl" $dst *> sta177.log
 if($LASTEXITCODE){throw 'Timing report failed'}
 $ErrorActionPreference='Stop'
 & "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $dst -AllowUnusedBridgeReady
} finally {Pop-Location}
if((Get-FileHash "$os/firmware.mif").Hash -ne (Get-FileHash "$dst/firmware.mif").Hash){throw 'MIF pair mismatch'}
Write-Host 'GB177 full compile and constrained timing PASS; NOT full I/O signoff or hardware validation'
