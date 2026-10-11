$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$src="$root/build/stream131-r8-timing4-quartus"
$dst="$root/build/gpu-lean132-quartus"
$q='C:/altera_lite/25.1std/quartus/bin64'
if(Test-Path -LiteralPath $dst){throw 'Preserve existing lean132 build'}
$null=New-Item -ItemType Directory -Path $dst
foreach($name in @('ap_core.qpf','ap_core.qsf','firmware.mif')){
 Copy-Item -LiteralPath "$src/$name" -Destination "$dst/$name"
}
Push-Location $dst
try {
 $ErrorActionPreference='Continue'
 & "$q/quartus_sh.exe" -t "$PSScriptRoot/configure-gpu-lean132.tcl" $dst *> lean-configure.log
 if($LASTEXITCODE){throw 'Lean configure failed'}
 & "$q/quartus_map.exe" ap_core *> lean-map.log
 if($LASTEXITCODE){throw 'Lean synthesis failed'}
} finally {$ErrorActionPreference='Stop';Pop-Location}
Get-Content "$dst/output_files/ap_core.map.summary"
