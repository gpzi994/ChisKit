$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..')).Replace('\','/')
$fit="$root/build/gb-compare208-quartus"
$os="$root/.integration/openfpgaos/src/firmware/os/bld/gb-rtc208"
$q='C:/altera_lite/25.1std/quartus/bin64'
if((Get-Content "$fit/output_files/ap_core.fit.summary" -Raw)-notmatch 'Fitter Status : Successful'){throw 'Fit incomplete'}
Copy-Item "$os/firmware.mif" "$fit/firmware.mif" -Force
Push-Location $fit
try{
 $ErrorActionPreference='Continue'
 & "$q/quartus_cdb.exe" ap_core --update_mif *> update-mif208.log
 if($LASTEXITCODE){throw 'MIF update failed'}
 & "$q/quartus_asm.exe" ap_core *> assembly-final208.log
 if($LASTEXITCODE){throw 'Assembly failed'}
 & "$q/quartus_sta.exe" -t "$PSScriptRoot/check-sdram124-sta.tcl" $fit *> sta208.log
 if($LASTEXITCODE){throw 'STA failed'}
 $ErrorActionPreference='Stop'
 & "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $fit -AllowUnusedBridgeReady
}finally{Pop-Location}
if((Get-FileHash "$os/firmware.mif").Hash-ne(Get-FileHash "$fit/firmware.mif").Hash){throw 'MIF mismatch'}
