$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$dst="$root/build/sram138-quartus"
$q='C:/altera_lite/25.1std/quartus/bin64'
if(Test-Path -LiteralPath $dst){throw 'Preserve existing build'}
$null=New-Item -ItemType Directory -Path $dst
foreach($n in @('ap_core.qpf','ap_core.qsf')){Copy-Item -LiteralPath "$root/build/timers135-quartus/$n" -Destination "$dst/$n"}
Copy-Item -LiteralPath "$root/.integration/openfpgaos/src/firmware/os/bld/sram138-release/firmware.mif" -Destination "$dst/firmware.mif"
Push-Location $dst
try {
 $ErrorActionPreference='Continue'
 & "$q/quartus_sh.exe" -t "$PSScriptRoot/configure-sram138.tcl" $dst *> configure.log
 if($LASTEXITCODE){throw 'Configure failed'}
 & "$q/quartus_sh.exe" --flow compile ap_core *> full-compile.log
 if($LASTEXITCODE){throw 'Full compilation failed'}
} finally {$ErrorActionPreference='Stop';Pop-Location}
Get-Content "$dst/output_files/ap_core.fit.summary"
Get-Content "$dst/output_files/ap_core.sta.summary"
