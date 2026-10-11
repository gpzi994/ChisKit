$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..')).Replace('\','/')
$src="$root/build/gb-rtc191-quartus"
$dst="$root/build/gb-compare208-quartus"
$os="$root/.integration/openfpgaos/src/firmware/os/bld/gb-rtc208"
$q='C:/altera_lite/25.1std/quartus/bin64'
if(Test-Path -LiteralPath $dst){throw 'Preserve existing experiment'}
New-Item -ItemType Directory -Path $dst | Out-Null
Get-ChildItem -LiteralPath $src -File | Where-Object {$_.Extension -in @('.qsf','.qpf','.sdc','.mif','.hex','.v','.sv','.vhd','.qip','.sip','.ip','.tcl')} | ForEach-Object {Copy-Item -LiteralPath $_.FullName -Destination $dst}
Copy-Item -LiteralPath "$os/firmware.mif" -Destination "$dst/firmware.mif"
$qsf=(Get-Content -LiteralPath "$dst/ap_core.qsf" -Raw).Replace($src,$dst)
$qsf=[regex]::Replace($qsf,'(?m)^set_global_assignment -name MIF_FILE .*$',('set_global_assignment -name MIF_FILE "'+$dst+'/firmware.mif"'))
$qsf+="`nset_global_assignment -name VERILOG_FILE `"$root/core/rtl/chisburner_gb_compare207.v`"`n"
[IO.File]::WriteAllText("$dst/ap_core.qsf",$qsf,[Text.UTF8Encoding]::new($false))
Push-Location $dst
try{
 $ErrorActionPreference='Continue'
 & "$q/quartus_sh.exe" --flow compile ap_core *> compile208.log
 if($LASTEXITCODE){throw 'Compile failed; inspect compile208.log'}
 & "$q/quartus_sta.exe" -t "$PSScriptRoot/check-sdram124-sta.tcl" $dst *> sta208.log
 if($LASTEXITCODE){throw 'STA failed'}
 $ErrorActionPreference='Stop'
 & "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $dst -AllowUnusedBridgeReady
}finally{Pop-Location}
if((Get-FileHash "$os/firmware.mif").Hash-ne(Get-FileHash "$dst/firmware.mif").Hash){throw 'MIF mismatch'}
Write-Host 'GB208 full compile + constrained timing PASS; external I/O NOT fully signed off.'
