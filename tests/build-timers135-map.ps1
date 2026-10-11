$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$src="$root/build/gb-identify134-quartus"
$dst="$root/build/timers135-quartus"
$q='C:/altera_lite/25.1std/quartus/bin64'
if(Test-Path -LiteralPath $dst){throw 'Preserve existing build'}
$null=New-Item -ItemType Directory -Path $dst
foreach($n in @('ap_core.qpf','ap_core.qsf','firmware.mif')){Copy-Item -LiteralPath "$src/$n" -Destination "$dst/$n"}
Push-Location $dst
try {
 $ErrorActionPreference='Continue'
 & "$q/quartus_sh.exe" -t "$PSScriptRoot/configure-timers135.tcl" $dst *> configure.log
 if($LASTEXITCODE){throw 'Configure failed'}
 & "$q/quartus_map.exe" ap_core *> map.log
 if($LASTEXITCODE){throw 'Synthesis failed'}
} finally {$ErrorActionPreference='Stop';Pop-Location}
Get-Content "$dst/output_files/ap_core.map.summary"
