[CmdletBinding()]
param([string]$QuartusBin = 'C:\altera_lite\25.1std\quartus\bin64', [switch]$Audit,
      [ValidatePattern('^[a-z]?$')][string]$AuditRevision='c')
$ErrorActionPreference = 'Stop'
$root = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$target = Join-Path $root '.integration\openfpgaos\src\fpga\targets\pocket'
$base = Join-Path $target 'bld\chisburner-storage-trace-s44-win'
$buildName = if ($Audit) { "bld\chisburner-storage-audit-s47$AuditRevision-win" } else { 'bld\chisburner-storage-events-s46-win' }
$dest = Join-Path $target $buildName
if (Test-Path -LiteralPath $dest) { throw 'Preserve existing event build' }
$boot = Join-Path $base 'firmware.mif'
if ((Get-FileHash -LiteralPath $boot).Hash.ToLowerInvariant() -ne 'aade051a2a517f71d55f1610a3b4e6379e73896c68c322a96556f3982befbba2') { throw 'Boot MIF mismatch' }
New-Item -ItemType Directory -Path (Join-Path $dest 'apf') -Force | Out-Null
Copy-Item -LiteralPath $boot -Destination (Join-Path $dest 'firmware.mif')
Copy-Item -LiteralPath (Join-Path $base 'ap_core.qpf') -Destination $dest
Copy-Item -LiteralPath (Join-Path $base 'apf\build_id.mif') -Destination (Join-Path $dest 'apf\build_id.mif')
$qsf = Get-Content -Raw -LiteralPath (Join-Path $base 'ap_core.qsf')
if ($qsf -match '(?m)^set_global_assignment -name VERILOG_MACRO INCLUDE_CARTRIDGE\s*$') { throw 'Cartridge profile forbidden' }
$mif = (Join-Path $dest 'firmware.mif').Replace('\','/')
$rtlName = if ($Audit) { 'core\storage\pct_bridge_audit.v' } else { 'core\storage\pct_bridge_trace.v' }
$rtl = (Join-Path $root $rtlName).Replace('\','/')
$qsf = [regex]::Replace($qsf,'(?m)^set_global_assignment -name MIF_FILE .*firmware\.mif\s*$', "set_global_assignment -name MIF_FILE $mif")
$qsf += "`nset_global_assignment -name VERILOG_MACRO CHISBURNER_STORAGE_EVENTS`nset_global_assignment -name VERILOG_FILE $rtl`n"
if ($Audit) { $qsf += "set_global_assignment -name VERILOG_MACRO CHISBURNER_STORAGE_AUDIT`n" }
[IO.File]::WriteAllText((Join-Path $dest 'ap_core.qsf'),$qsf,(New-Object Text.UTF8Encoding($false)))
$sources = @('core_top.v','core_bridge_cmd.v','apf\apf_top.v','apf\io_bridge_peripheral.v','..\..\common\axi_periph_slave.v') | ForEach-Object { Join-Path $target $_ }
$sources += @($rtl,(Join-Path $dest 'firmware.mif'),(Join-Path $dest 'apf\build_id.mif'))
$hashes = $sources | ForEach-Object { "$( (Get-FileHash -LiteralPath $_).Hash.ToLowerInvariant())  $([IO.Path]::GetFullPath($_))" }
[IO.File]::WriteAllLines((Join-Path $dest 'SOURCE_SHA256.txt'),[string[]]$hashes)
Push-Location $dest
try {
    & (Join-Path $QuartusBin 'quartus_sh.exe') --flow compile ap_core *> compile.log
    if ($LASTEXITCODE -ne 0) { throw "Compile failed: $dest\compile.log" }
    & (Join-Path $QuartusBin 'quartus_cdb.exe') ap_core --update_mif *> mif-update.log
    if ($LASTEXITCODE -ne 0) { throw 'MIF update failed' }
    & (Join-Path $QuartusBin 'quartus_asm.exe') ap_core *> assembler.log
    if ($LASTEXITCODE -ne 0) { throw 'Assembler failed' }
} finally { Pop-Location }
$timing = Get-Content -Raw -LiteralPath (Join-Path $dest 'output_files\ap_core.sta.rpt')
foreach ($kind in @('setup','hold')) {
    $matches = [regex]::Matches($timing,"Worst-case $kind slack is\s+(-?\d+(?:\.\d+)?)")
    if (!$matches.Count) { throw "Missing $kind result" }
    $min = ($matches | ForEach-Object { [double]$_.Groups[1].Value } | Measure-Object -Minimum).Minimum
    if ($min -le 0) { throw "Timing failed: $kind $min" }
    "$kind slack: $min ns"
}
'Event FPGA compile complete'
