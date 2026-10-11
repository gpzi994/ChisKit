$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$src="$root/build/rtc167-r2-quartus"
$dst="$root/build/flash170-quartus"
$os="$root/.integration/openfpgaos/src/firmware/os/bld/flash170"
$q='C:/altera_lite/25.1std/quartus/bin64'
if(Test-Path $dst){throw 'Preserve existing experiment'}
& "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $src -AllowUnusedBridgeReady
Copy-Item $src $dst -Recurse
Copy-Item "$os/firmware.mif" "$dst/firmware.mif" -Force
# Mechanical relocation only; no RTL or routing edits.
$qsf=Get-Content "$dst/ap_core.qsf" -Raw
$qsf=$qsf.Replace($src.Replace('\','/'),$dst.Replace('\','/'))
[IO.File]::WriteAllText("$dst/ap_core.qsf",$qsf,[Text.UTF8Encoding]::new($false))
Push-Location $dst
try{
 $ErrorActionPreference='Continue' # Quartus emits a benign allocator notice on stderr.
 & "$q/quartus_cdb.exe" ap_core --update_mif *> mif-update-170.log
 if($LASTEXITCODE){throw 'MIF update failed'}
 & "$q/quartus_asm.exe" ap_core *> asm-170.log
 if($LASTEXITCODE){throw 'Assembler failed'}
 & "$q/quartus_sta.exe" -t "$PSScriptRoot/check-sdram124-sta.tcl" $dst *> sta-170.log
 if($LASTEXITCODE){throw 'Timing report failed'}
}finally{Pop-Location}
$ErrorActionPreference='Stop'
& "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $dst -AllowUnusedBridgeReady
if((Get-FileHash "$os/firmware.mif").Hash -ne (Get-FileHash "$dst/firmware.mif").Hash){throw 'Pair mismatch'}
Write-Host 'PASS matched MIF, assembly and constrained timing; hardware boot pending'
