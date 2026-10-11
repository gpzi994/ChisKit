$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$src="$root/build/flash170-quartus"
$dst="$root/build/save171-r3-quartus"
$os="$root/.integration/openfpgaos/src/firmware/os/bld/save171"
$q='C:/altera_lite/25.1std/quartus/bin64'
if(Test-Path $dst){throw 'Preserve existing experiment'}
& "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $src -AllowUnusedBridgeReady
Copy-Item $src $dst -Recurse
Copy-Item "$os/firmware.mif" "$dst/firmware.mif" -Force
# Mechanical path relocation in the experiment; RTL and route are unchanged.
$qsf=Get-Content "$dst/ap_core.qsf" -Raw
$qsf=$qsf.Replace($src.Replace('\','/'),$dst.Replace('\','/'))
[IO.File]::WriteAllText("$dst/ap_core.qsf",$qsf,[Text.UTF8Encoding]::new($false))
Push-Location $dst
try{
 $ErrorActionPreference='Continue'
 & "$q/quartus_cdb.exe" ap_core --update_mif *> mif-update-171.log
 if($LASTEXITCODE){throw 'MIF update failed'}
 & "$q/quartus_sta.exe" -t "$PSScriptRoot/check-sdram124-sta.tcl" $dst *> sta-171.log
 if($LASTEXITCODE){throw 'Timing report failed'}
 $ErrorActionPreference='Stop'
 & "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $dst -AllowUnusedBridgeReady
 $ErrorActionPreference='Continue'
 & "$q/quartus_asm.exe" ap_core *> asm-171.log
 if($LASTEXITCODE){throw 'Assembler failed'}
}finally{Pop-Location}
$ErrorActionPreference='Stop'
if((Get-FileHash "$os/firmware.mif").Hash -ne (Get-FileHash "$dst/firmware.mif").Hash){throw 'Pair mismatch'}
Write-Host 'PASS matched MIF, constrained timing and assembly; hardware boot pending'
