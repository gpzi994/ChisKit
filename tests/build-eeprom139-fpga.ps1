param([switch]$Resume,[switch]$AutoSave,[ValidatePattern('^[a-zA-Z0-9_-]+$')][string]$BuildName='eeprom139-quartus')
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$dst="$root/build/$BuildName"
$q='C:/altera_lite/25.1std/quartus/bin64'
if((Test-Path -LiteralPath $dst) -and !$Resume){throw 'Preserve existing build; use -Resume explicitly'}
if(!$Resume){
$null=New-Item -ItemType Directory -Path $dst
foreach($n in @('ap_core.qpf','ap_core.qsf')){Copy-Item -LiteralPath "$root/build/sram138-quartus/$n" -Destination "$dst/$n"}
Copy-Item -LiteralPath "$root/.integration/openfpgaos/src/firmware/os/bld/eeprom139-integration/firmware.mif" -Destination "$dst/firmware.mif"
}
Push-Location $dst
try {
 $ErrorActionPreference='Continue' # Native stderr warnings are not process failure.
 $mode=if($AutoSave){'auto-save'}else{'diagnostic'}
 & "$q/quartus_sh.exe" -t "$PSScriptRoot/configure-eeprom139.tcl" $dst $mode *> configure.log
 if($LASTEXITCODE){throw 'Configure failed'}
 & "$q/quartus_sh.exe" --flow compile ap_core *> full-compile.log
 if($LASTEXITCODE){throw 'Full compilation failed'}
} finally {$ErrorActionPreference='Stop';Pop-Location}
Get-Content "$dst/output_files/ap_core.fit.summary"
Get-Content "$dst/output_files/ap_core.sta.summary"
