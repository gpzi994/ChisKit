$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$fit="$root/build/gb-restore180-r2-quartus"
$os="$root/.integration/openfpgaos/src/firmware/os/bld/gb-restore180-r2"
$sourceFit="$root/build/gb-restore180-quartus"
if(Test-Path -LiteralPath $fit){throw 'Preserve previous r2 assembly'}
Copy-Item -LiteralPath $sourceFit -Destination $fit -Recurse
$qsf=Get-Content -LiteralPath "$fit/ap_core.qsf" -Raw
$qsf=$qsf.Replace($sourceFit,$fit)
[IO.File]::WriteAllText("$fit/ap_core.qsf",$qsf,[Text.UTF8Encoding]::new($false))
$q='C:/altera_lite/25.1std/quartus/bin64'
if((Get-Content "$fit/output_files/ap_core.fit.summary" -Raw)-notmatch 'Fitter Status : Successful'){throw 'Full GB180-r2 fit required first'}
if(!(Test-Path "$fit/firmware-full-fit.mif")){Copy-Item -LiteralPath "$fit/firmware.mif" -Destination "$fit/firmware-full-fit.mif"}
Copy-Item -LiteralPath "$os/firmware.mif" -Destination "$fit/firmware.mif" -Force
Push-Location $fit
try{
 $ErrorActionPreference='Continue'
 & "$q/quartus_cdb.exe" ap_core --update_mif *> mif-final180r2.log
 if($LASTEXITCODE){throw 'Final MIF update failed'}
 & "$q/quartus_asm.exe" ap_core *> asm-final180r2.log
 if($LASTEXITCODE){throw 'Final assembler failed'}
 & "$q/quartus_sta.exe" -t "$PSScriptRoot/check-sdram124-sta.tcl" $fit *> sta-final180r2.log
 if($LASTEXITCODE){throw 'Final STA failed'}
}finally{Pop-Location}
$ErrorActionPreference='Stop'
& "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $fit -AllowUnusedBridgeReady
if((Get-FileHash "$os/firmware.mif").Hash-ne(Get-FileHash "$fit/firmware.mif").Hash){throw 'Final MIF mismatch'}
Write-Host 'GB180-r2 final software MIF/assembly/STA PASS; uses this revision full placement and routing'
