param([switch]$Reassemble)
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$fit="$root/build/gb-mbc2-restore184-quartus"
$os="$root/.integration/openfpgaos/src/firmware/os/bld/gb-mbc2-restore184"
$base="$root/build/gb-restore180-r2-quartus"
if((Test-Path -LiteralPath $fit)-and !$Reassemble){throw 'Preserve existing assembly'}
& "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $base -AllowUnusedBridgeReady
if(!(Test-Path -LiteralPath $fit)){Copy-Item -LiteralPath $base -Destination $fit -Recurse}
$qsf=(Get-Content -LiteralPath "$fit/ap_core.qsf" -Raw).Replace($base,$fit)
[IO.File]::WriteAllText("$fit/ap_core.qsf",$qsf,[Text.UTF8Encoding]::new($false))
Copy-Item -LiteralPath "$os/firmware.mif" -Destination "$fit/firmware.mif" -Force
$q='C:/altera_lite/25.1std/quartus/bin64'
Push-Location $fit
try{
 $ErrorActionPreference='Continue'
 & "$q/quartus_cdb.exe" ap_core --update_mif *> mif184.log
 if($LASTEXITCODE){throw 'MIF update failed'}
 & "$q/quartus_asm.exe" ap_core *> asm184.log
 if($LASTEXITCODE){throw 'Assembly failed'}
 & "$q/quartus_sta.exe" -t "$PSScriptRoot/check-sdram124-sta.tcl" $fit *> sta184.log
 if($LASTEXITCODE){throw 'STA failed'}
}finally{Pop-Location}
$ErrorActionPreference='Stop'
& "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $fit -AllowUnusedBridgeReady
if((Get-FileHash "$os/firmware.mif").Hash-ne(Get-FileHash "$fit/firmware.mif").Hash){throw 'MIF mismatch'}
Write-Host 'GB184 firmware-only assembly and constrained timing checked; inherited GB180 placement/routing, not a new full fit.'
