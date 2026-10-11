$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$src="$root/build/gb-save177-quartus"
$dst="$root/build/gb-save178-quartus"
$os="$root/.integration/openfpgaos/src/firmware/os/bld/gb-save178"
$q='C:/altera_lite/25.1std/quartus/bin64'
if(Test-Path -LiteralPath $dst){throw 'Preserve existing experiment'}
& "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $src -AllowUnusedBridgeReady
# Software-only revision. Preserve routed database; update initialized memory only.
Copy-Item -LiteralPath $src -Destination $dst -Recurse
Copy-Item -LiteralPath "$os/firmware.mif" -Destination "$dst/firmware.mif" -Force
$qsf=Get-Content -LiteralPath "$dst/ap_core.qsf" -Raw
$qsf=$qsf.Replace($src.Replace('\','/'),$dst.Replace('\','/'))
[IO.File]::WriteAllText("$dst/ap_core.qsf",$qsf,[Text.UTF8Encoding]::new($false))
Push-Location $dst
try{
 $ErrorActionPreference='Continue'
 & "$q/quartus_cdb.exe" ap_core --update_mif *> mif-update178.log
 if($LASTEXITCODE){throw 'MIF update failed'}
 & "$q/quartus_asm.exe" ap_core *> asm178.log
 if($LASTEXITCODE){throw 'Assembler failed'}
 & "$q/quartus_sta.exe" -t "$PSScriptRoot/check-sdram124-sta.tcl" $dst *> sta178.log
 if($LASTEXITCODE){throw 'Timing report failed'}
}finally{Pop-Location}
$ErrorActionPreference='Stop'
& "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $dst -AllowUnusedBridgeReady
if((Get-FileHash "$os/firmware.mif").Hash-ne(Get-FileHash "$dst/firmware.mif").Hash){throw 'MIF mismatch'}
Write-Host 'GB178 MIF update/assembly/constrained timing PASS; reuses GB177 routing, hardware test pending'
