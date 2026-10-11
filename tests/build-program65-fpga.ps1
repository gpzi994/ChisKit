[CmdletBinding()]
param(
    [string]$QuartusBin='C:/altera_lite/25.1std/quartus/bin64',
    [int]$Seed=7,
    [switch]$AggressiveRoutability,
    [switch]$MaximumPlacementEffort,
    [switch]$AggressivePerformance,
    [switch]$PhysicalSynthesis,
    [string]$OsBuild='.integration/openfpgaos/src/firmware/os/bld/program66-pocket-release',
    [string]$BuildTag='capfix66'
)

$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$target=Join-Path $root '.integration/openfpgaos/src/fpga/targets/pocket'
$base=Join-Path $target 'bld/chisburner-dump57-s7-win'
$suffix=if($AggressiveRoutability){'-route'}else{''}
if($MaximumPlacementEffort){$suffix+='-maxfit'}
if($AggressivePerformance){$suffix+='-perf'}
if($PhysicalSynthesis){$suffix+='-phys'}
$tag=if($BuildTag){'-'+$BuildTag}else{''}
$dest=Join-Path $target "bld/chisburner-program65-s$Seed$suffix$tag-win"
$osRoot=if([IO.Path]::IsPathRooted($OsBuild)){$OsBuild}else{Join-Path $root $OsBuild}
$utf8=[Text.UTF8Encoding]::new($false)

if(Test-Path -LiteralPath $dest){throw 'Preserve existing program65 FPGA build'}
foreach($p in @($base,(Join-Path $base 'ap_core.qsf'),
                (Join-Path $base 'ap_core.qpf'),
                (Join-Path $base 'apf/build_id.mif'),
                (Join-Path $osRoot 'firmware.mif'))){
    if(!(Test-Path -LiteralPath $p)){throw "Missing build input $p"}
}

New-Item -ItemType Directory -Path (Join-Path $dest 'apf') -Force | Out-Null
Copy-Item -LiteralPath (Join-Path $base 'ap_core.qpf') -Destination $dest
Copy-Item -LiteralPath (Join-Path $base 'apf/build_id.mif') `
    -Destination (Join-Path $dest 'apf/build_id.mif')
Copy-Item -LiteralPath (Join-Path $osRoot 'firmware.mif') `
    -Destination (Join-Path $dest 'firmware.mif')

$qsf=(Get-Content -Raw (Join-Path $base 'ap_core.qsf')).Replace("`r`n","`n")
$top=(Join-Path $target 'core_top.v').Replace('\','/')
$programmer=(Join-Path $target 'chisburner_cart_gba_s29gl256.v').Replace('\','/')
$mif=(Join-Path $dest 'firmware.mif').Replace('\','/')
$qsf=[regex]::Replace($qsf,
    '(?m)^set_global_assignment -name VERILOG_FILE .*core_top[^\r\n]*\.v\s*$',
    "set_global_assignment -name VERILOG_FILE $top")
$qsf=[regex]::Replace($qsf,
    '(?m)^set_global_assignment -name MIF_FILE .*firmware\.mif\s*$',
    "set_global_assignment -name MIF_FILE $mif")
$qsf=[regex]::Replace($qsf,
    '(?m)^set_global_assignment -name VERILOG_MACRO CHISBURNER_GBA_IDENT56\s*\n?', '')
$qsf=[regex]::Replace($qsf,
    '(?m)^set_global_assignment -name VERILOG_MACRO CHISBURNER_GBA_PROGRAM65\s*\n?', '')
foreach($macro in @('CHISBURNER_STORAGE_TRACE','CHISBURNER_STORAGE_EVENTS',
                    'CHISBURNER_STORAGE_AUDIT')) {
    $qsf=[regex]::Replace($qsf,
        "(?m)^set_global_assignment -name VERILOG_MACRO $macro\s*\n?", '')
}
$qsf=[regex]::Replace($qsf,
    '(?m)^set_global_assignment -name SEED \d+\s*$',
    "set_global_assignment -name SEED $Seed")
if($qsf -notmatch [regex]::Escape($programmer)){
    $qsf += "`nset_global_assignment -name VERILOG_FILE $programmer`n"
}
if($qsf -notmatch '(?m)^set_global_assignment -name VERILOG_MACRO INCLUDE_CARTRIDGE\s*$'){
    $qsf += "set_global_assignment -name VERILOG_MACRO INCLUDE_CARTRIDGE`n"
}
$qsf += "set_global_assignment -name VERILOG_MACRO CHISBURNER_GBA_PROGRAM65`n"
if($AggressiveRoutability){
    $qsf += "set_global_assignment -name FITTER_AGGRESSIVE_ROUTABILITY_OPTIMIZATION ALWAYS`n"
}
if($MaximumPlacementEffort){
    $qsf=[regex]::Replace($qsf,
        '(?m)^set_global_assignment -name FITTER_EFFORT .+$',
        'set_global_assignment -name FITTER_EFFORT "STANDARD FIT"')
    $qsf += "`nset_global_assignment -name PLACEMENT_EFFORT_MULTIPLIER 3.0`n"
}
if($AggressivePerformance){
    $qsf=[regex]::Replace($qsf,
        '(?m)^set_global_assignment -name OPTIMIZATION_MODE .+$',
        'set_global_assignment -name OPTIMIZATION_MODE "AGGRESSIVE PERFORMANCE"')
}
if($PhysicalSynthesis){
    $qsf=[regex]::Replace($qsf,
        '(?m)^set_global_assignment -name PHYSICAL_SYNTHESIS_COMBO_LOGIC .+$',
        'set_global_assignment -name PHYSICAL_SYNTHESIS_COMBO_LOGIC ON')
    $qsf=[regex]::Replace($qsf,
        '(?m)^set_global_assignment -name PHYSICAL_SYNTHESIS_REGISTER_RETIMING .+$',
        'set_global_assignment -name PHYSICAL_SYNTHESIS_REGISTER_RETIMING OFF')
    $qsf=[regex]::Replace($qsf,
        '(?m)^set_global_assignment -name PHYSICAL_SYNTHESIS_EFFORT .+$',
        'set_global_assignment -name PHYSICAL_SYNTHESIS_EFFORT EXTRA')
}
[IO.File]::WriteAllText((Join-Path $dest 'ap_core.qsf'),$qsf,$utf8)

$sources=@($top,$programmer,(Join-Path $target 'chisburner_cart_gba_readonly.v'),
           (Join-Path $root 'core/storage/pct_bridge_audit.v'),
           (Join-Path $root '.integration/openfpgaos/src/fpga/common/axi_periph_slave.v'),
           (Join-Path $target 'core_top.v'),(Join-Path $dest 'firmware.mif'),
           (Join-Path $dest 'ap_core.qsf'))
[IO.File]::WriteAllLines((Join-Path $dest 'PROGRAM65_SHA256.txt'),[string[]]@(
    $sources|ForEach-Object{
        $full=[IO.Path]::GetFullPath($_)
        "$((Get-FileHash -LiteralPath $full).Hash.ToLowerInvariant())  $full"
    }),$utf8)

Push-Location $dest
try{
    & (Join-Path $QuartusBin 'quartus_sh.exe') --flow compile ap_core *> compile.log
    if($LASTEXITCODE -ne 0){throw "FPGA compile failed $dest/compile.log"}
    & (Join-Path $QuartusBin 'quartus_cdb.exe') ap_core --update_mif *> mif-update.log
    if($LASTEXITCODE -ne 0){throw 'MIF update failed'}
    & (Join-Path $QuartusBin 'quartus_asm.exe') ap_core *> assembler.log
    if($LASTEXITCODE -ne 0){throw 'Assembler failed'}
} finally {
    Pop-Location
}

$sta=Get-Content -Raw (Join-Path $dest 'output_files/ap_core.sta.rpt')
foreach($kind in @('setup','hold')){
    $m=[regex]::Matches($sta,"Worst-case $kind slack is\s+(-?\d+(?:\.\d+)?)")
    if(!$m.Count){throw "Missing $kind timing result"}
    $slack=($m|ForEach-Object{[double]$_.Groups[1].Value}|
        Measure-Object -Minimum).Minimum
    if($slack -le 0){throw "Timing failed $kind $slack ns"}
    "$kind slack $slack ns"
}
"Program65 FPGA ready: $dest"
