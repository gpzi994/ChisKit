[CmdletBinding()]
param([string]$QuartusBin = 'C:\altera_lite\25.1std\quartus\bin64')
$ErrorActionPreference = 'Stop'
$projectRoot = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$target = Join-Path $projectRoot '.integration\openfpgaos\src\fpga\targets\pocket'
$baseBuild = Join-Path $target 'bld\chisburner-storage-stream-s2-win'
$traceBuild = Join-Path $target 'bld\chisburner-storage-trace-s44-win'
if (Test-Path -LiteralPath $traceBuild) { throw 'Trace build already exists; preserve it' }
$firmware = Join-Path $projectRoot 'build\storage-stream-artifacts\firmware.mif'
$expected = 'aade051a2a517f71d55f1610a3b4e6379e73896c68c322a96556f3982befbba2'
if ((Get-FileHash -LiteralPath $firmware).Hash.ToLowerInvariant() -ne $expected) {
    throw 'Known-good boot firmware hash mismatch'
}
$qsf = Get-Content -Raw -LiteralPath (Join-Path $baseBuild 'ap_core.qsf')
if ($qsf -notmatch '(?m)^set_global_assignment -name VERILOG_MACRO CHISBURNER_STORAGE_STREAM\s*$' -or
    $qsf -match '(?m)^set_global_assignment -name VERILOG_MACRO INCLUDE_CARTRIDGE\s*$') {
    throw 'Not an empty-slot storage hardware profile'
}
New-Item -ItemType Directory -Path (Join-Path $traceBuild 'apf') -Force | Out-Null
Copy-Item -LiteralPath $firmware -Destination (Join-Path $traceBuild 'firmware.mif')
Copy-Item -LiteralPath (Join-Path $baseBuild 'ap_core.qpf') -Destination $traceBuild
Copy-Item -LiteralPath (Join-Path $baseBuild 'apf\build_id.mif') -Destination (Join-Path $traceBuild 'apf\build_id.mif')
$localMif = (Join-Path $traceBuild 'firmware.mif').Replace('\', '/')
$qsf = [regex]::Replace($qsf, '(?m)^set_global_assignment -name MIF_FILE .*firmware\.mif\s*$',
    "set_global_assignment -name MIF_FILE $localMif")
$qsf += "`nset_global_assignment -name VERILOG_MACRO CHISBURNER_STORAGE_TRACE`n"
[IO.File]::WriteAllText((Join-Path $traceBuild 'ap_core.qsf'), $qsf, (New-Object Text.UTF8Encoding($false)))

# Preserve exact source identities for this compile. Do not alter the target's
# live firmware.mif, which may belong to a different OS build.
$sources = @(
    (Join-Path $target 'core_bridge_cmd.v'), (Join-Path $target 'core_top.v'),
    (Join-Path $target '..\..\common\axi_periph_slave.v'),
    (Join-Path $target 'core_constraints.sdc'), (Join-Path $traceBuild 'firmware.mif'),
    (Join-Path $traceBuild 'apf\build_id.mif')
)
$hashes = $sources | ForEach-Object { "$( (Get-FileHash -LiteralPath $_).Hash.ToLowerInvariant())  $([IO.Path]::GetFullPath($_))" }
[IO.File]::WriteAllLines((Join-Path $traceBuild 'SOURCE_SHA256.txt'), [string[]]$hashes)
Push-Location $traceBuild
try {
    & (Join-Path $QuartusBin 'quartus_sh.exe') --flow compile ap_core *> compile.log
    if ($LASTEXITCODE -ne 0) { throw "Quartus compile failed; see $traceBuild\compile.log" }
    & (Join-Path $QuartusBin 'quartus_cdb.exe') ap_core --update_mif *> mif-update.log
    if ($LASTEXITCODE -ne 0) { throw 'Quartus MIF update failed' }
    & (Join-Path $QuartusBin 'quartus_asm.exe') ap_core *> assembler.log
    if ($LASTEXITCODE -ne 0) { throw 'Quartus assembler failed' }
} finally { Pop-Location }
$timing = Get-Content -Raw -LiteralPath (Join-Path $traceBuild 'output_files\ap_core.sta.rpt')
foreach ($kind in @('setup', 'hold')) {
    $matches = [regex]::Matches($timing, "Worst-case $kind slack is\s+(-?\d+(?:\.\d+)?)")
    if ($matches.Count -eq 0) { throw "Missing $kind timing result" }
    $min = ($matches | ForEach-Object { [double]$_.Groups[1].Value } | Measure-Object -Minimum).Minimum
    if ($min -le 0) { throw "Non-positive $kind slack: $min" }
    Write-Output "$kind slack: $min ns"
}
$update = Get-Content -Raw -LiteralPath (Join-Path $traceBuild 'output_files\ap_core.mif_update.rpt')
if ($update -notmatch 'Processed Memory Initialization File .*firmware\.mif' -or
    $update -notmatch 'Processed Memory Initialization File .*build_id\.mif' -or
    $update -match 'Critical Warning|can.t find|setting all initial values to 0') {
    throw 'MIF update did not verify both boot/build-ID memories'
}
Write-Output "Trace FPGA ready: $traceBuild"
