param([ValidateSet('r4','r5')][string]$Revision='r4',[switch]$Resume)
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$base="$root/build/save160-quartus"
$dst="$root/build/save160-$Revision-quartus"
if(!$Resume){
 if(Test-Path -LiteralPath $dst){throw 'Preserve existing build'}
 Copy-Item -LiteralPath $base -Destination $dst -Recurse
}elseif(!(Test-Path "$dst/ap_core.qsf")){throw 'No existing build to resume'}
$qsf=(Get-Content "$dst/ap_core.qsf" -Raw).Replace($base.Replace('\','/'),$dst.Replace('\','/'))
[IO.File]::WriteAllText("$dst/ap_core.qsf",$qsf,[Text.UTF8Encoding]::new($false))
Copy-Item "$root/.integration/openfpgaos/src/firmware/os/bld/save160-$Revision/firmware.mif" "$dst/firmware.mif" -Force
Push-Location $dst
try{
 & 'C:/altera_lite/25.1std/quartus/bin64/quartus_cdb.exe' ap_core --update_mif *> mif-update.log
 if($LASTEXITCODE){throw 'MIF update failed'}
 & 'C:/altera_lite/25.1std/quartus/bin64/quartus_asm.exe' ap_core *> assembler.log
 if($LASTEXITCODE){throw 'Assembler failed'}
}finally{Pop-Location}
& 'C:/altera_lite/25.1std/quartus/bin64/quartus_sta.exe' -t "$root/tests/check-sdram124-sta.tcl" $dst *> "$dst/scoped-sta.log"
if($LASTEXITCODE){throw 'STA failed'}
& "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $dst
