param([switch]$Resume)
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$src="$root/build/auto149-quartus"
$dst="$root/build/save160-quartus"
$mif="$root/.integration/openfpgaos/src/firmware/os/bld/save160-integration/firmware.mif"
$quartus='C:/altera_lite/25.1std/quartus/bin64'
if(!(Test-Path -LiteralPath $mif)){throw 'Build save160 OS first'}
if(Test-Path -LiteralPath $dst){if(!$Resume){throw "Preserve existing build: $dst"}}
else {
    $null=New-Item -ItemType Directory -Path $dst
    foreach($name in @('ap_core.qpf','ap_core.qsf')){
        Copy-Item -LiteralPath "$src/$name" -Destination "$dst/$name"
    }
}
Copy-Item -LiteralPath $mif -Destination "$dst/firmware.mif" -Force
Push-Location $dst
try {
    $ErrorActionPreference='Continue'
    & "$quartus/quartus_sh.exe" -t "$PSScriptRoot/configure-save160.tcl" $dst *> configure.log
    if($LASTEXITCODE){throw 'save160 configure failed'}
    & "$quartus/quartus_sh.exe" --flow compile ap_core *> full-compile.log
    if($LASTEXITCODE){throw 'save160 full FPGA compilation failed'}
    & "$quartus/quartus_sta.exe" -t "$PSScriptRoot/check-sdram124-sta.tcl" $dst *> cdc-report.log
    if($LASTEXITCODE){throw 'save160 timing and CDC report generation failed'}
} finally {$ErrorActionPreference='Stop';Pop-Location}
& "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $dst
Get-Content "$dst/output_files/ap_core.fit.summary"
Get-Content "$dst/output_files/ap_core.sta.summary"
