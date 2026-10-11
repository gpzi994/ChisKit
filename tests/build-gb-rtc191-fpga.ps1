$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$src="$root/build/gb-rtc190-quartus"
$dst="$root/build/gb-rtc191-quartus"
$os="$root/.integration/openfpgaos/src/firmware/os/bld/gb-rtc191"
$q='C:/altera_lite/25.1std/quartus/bin64'
if(Test-Path -LiteralPath $dst){throw 'Preserve existing experiment'}
& "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $src -AllowUnusedBridgeReady
New-Item -ItemType Directory -Path $dst | Out-Null
Get-ChildItem -LiteralPath $src -File | Where-Object {$_.Extension -in @('.qsf','.qpf','.sdc','.mif','.hex','.v','.sv','.vhd','.qip','.sip','.ip','.tcl')} | ForEach-Object {Copy-Item -LiteralPath $_.FullName -Destination $dst}
Copy-Item -LiteralPath "$os/firmware.mif" -Destination "$dst/firmware.mif"
$qsf=(Get-Content -LiteralPath "$dst/ap_core.qsf" -Raw).Replace($src,$dst)
$qsf=[regex]::Replace($qsf,'(?m)^set_global_assignment -name MIF_FILE .*$',('set_global_assignment -name MIF_FILE "'+$dst+'/firmware.mif"'))
$qsf+="`nset_global_assignment -name VERILOG_MACRO CHISBURNER_GB_RTC191`n"
[IO.File]::WriteAllText("$dst/ap_core.qsf",$qsf,[Text.UTF8Encoding]::new($false))
Push-Location $dst
try{
 $ErrorActionPreference='Continue'
 & "$q/quartus_sh.exe" --flow compile ap_core *> compile191.log
 if($LASTEXITCODE){throw 'Compile failed; inspect compile191.log'}
 & "$q/quartus_sta.exe" -t "$PSScriptRoot/check-sdram124-sta.tcl" $dst *> sta191.log
 if($LASTEXITCODE){throw 'STA failed'}
 $ErrorActionPreference='Stop'
 & "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $dst -AllowUnusedBridgeReady
}finally{Pop-Location}
if((Get-FileHash "$os/firmware.mif").Hash-ne(Get-FileHash "$dst/firmware.mif").Hash){throw 'MIF mismatch'}
Write-Host 'GB191 full compile + constrained timing PASS; external I/O NOT fully signed off.'
