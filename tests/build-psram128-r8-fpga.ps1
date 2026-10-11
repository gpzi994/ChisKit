$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$dst="$root/build/psram128-r8/quartus"
if(Test-Path $dst){throw 'Preserve existing r8 build'}
Copy-Item "$root/build/psram128-r7-seed2/quartus" $dst -Recurse
Copy-Item "$root/.integration/openfpgaos/src/firmware/os/bld/psram128/firmware.mif" "$dst/firmware.mif" -Force
$q='C:/altera_lite/25.1std/quartus/bin64'
Push-Location $dst
try {
 $ErrorActionPreference='Continue' # Native Quartus allocator warning is stderr.
 & "$q/quartus_cdb.exe" ap_core --update_mif *> update-mif.log
 if($LASTEXITCODE){throw 'MIF update failed'}
 & "$q/quartus_asm.exe" ap_core *> assemble.log
 if($LASTEXITCODE){throw 'Assembly failed'}
} finally {Pop-Location}
