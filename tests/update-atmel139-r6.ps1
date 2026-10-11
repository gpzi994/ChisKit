param([ValidateSet('r6','stages141','read143','read144','read145','wait146','auto149')][string]$Revision='r6')
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$base="$root/build/eeprom139-auto-r5-quartus"
$dst=if($Revision -eq 'r6'){"$root/build/eeprom139-auto-r6-quartus"}else{"$root/build/stages141-quartus"}
if($Revision -eq 'read143'){$dst="$root/build/read143-quartus"}
if($Revision -eq 'read144'){$dst="$root/build/read144-quartus"}
if($Revision -eq 'read145'){$dst="$root/build/read145-quartus"}
if($Revision -eq 'wait146'){$dst="$root/build/wait146-quartus"}
if($Revision -eq 'auto149'){$dst="$root/build/auto149-quartus"}
if(Test-Path -LiteralPath $dst){throw 'Preserve existing build'}
& "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $base
Copy-Item -LiteralPath $base -Destination $dst -Recurse
$qsf=(Get-Content "$dst/ap_core.qsf" -Raw).Replace($base.Replace('\','/'),$dst.Replace('\','/'))
[IO.File]::WriteAllText("$dst/ap_core.qsf",$qsf,[Text.UTF8Encoding]::new($false))
Copy-Item -LiteralPath "$root/.integration/openfpgaos/src/firmware/os/bld/eeprom139-integration/firmware.mif" -Destination "$dst/firmware.mif" -Force
Push-Location $dst
try {
 & 'C:/altera_lite/25.1std/quartus/bin64/quartus_cdb.exe' ap_core --update_mif *> mif-update.log
 if($LASTEXITCODE){throw 'MIF update failed'}
 & 'C:/altera_lite/25.1std/quartus/bin64/quartus_asm.exe' ap_core *> assembler.log
 if($LASTEXITCODE){throw 'Assembly failed'}
}finally{Pop-Location}
& 'C:/altera_lite/25.1std/quartus/bin64/quartus_sta.exe' -t "$root/tests/check-sdram124-sta.tcl" $dst *> "$dst/scoped-sta.log"
if($LASTEXITCODE){throw 'STA failed'}
& "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $dst
