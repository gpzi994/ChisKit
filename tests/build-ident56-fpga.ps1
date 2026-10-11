[CmdletBinding()]
param([string]$QuartusBin='C:/altera_lite/25.1std/quartus/bin64',[int]$Seed=4)
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$target=Join-Path $root '.integration/openfpgaos/src/fpga/targets/pocket'
$base=Join-Path $target 'bld/chisburner-storage-audit-s47c-win'
$dest=Join-Path $target "bld/chisburner-ident56-s$Seed-win"
if(Test-Path -LiteralPath $dest){throw 'Preserve existing FPGA build'}
foreach($line in Get-Content (Join-Path $base 'SOURCE_SHA256.txt')){
 if($line -notmatch '^([0-9a-f]{64})  (.+)$'){throw 'Bad source manifest'}
 $leaf=[IO.Path]::GetFileName($Matches[2])
 if($leaf -notin @('core_top.v','chisburner_cart_gba_readonly.v') -and
    (Get-FileHash -LiteralPath $Matches[2]).Hash.ToLowerInvariant() -ne $Matches[1]){
    throw "Unrelated source drift $line"
 }
}
$utf8=New-Object Text.UTF8Encoding($false)
New-Item -ItemType Directory -Path (Join-Path $dest 'apf') -Force | Out-Null
foreach($f in @('firmware.mif','ap_core.qpf','apf/build_id.mif')){
 Copy-Item -LiteralPath (Join-Path $base $f) -Destination (Join-Path $dest $f)
}
$top=Get-Content -Raw (Join-Path $target 'core_top.v')
$marker='`else'+"`n"+'localparam integer CHISBURNER_READS_ENABLED       = 0;'
$top=$top.Replace("`r`n","`n")
if(($top.Split(@($marker),[StringSplitOptions]::None)).Count -ne 2){throw 'Unexpected core_top profile marker'}
$policy=Get-Content -Raw (Join-Path $root 'core/cartridge_hal/identify56_policy.vh')
$top=$top.Replace($marker,'`elsif CHISBURNER_GBA_IDENT56'+"`n"+$policy+"`n"+$marker)
[IO.File]::WriteAllText((Join-Path $dest 'core_top_ident56.v'),$top,$utf8)
$qsf=Get-Content -Raw (Join-Path $base 'ap_core.qsf')
if($qsf -match 'VERILOG_MACRO INCLUDE_CARTRIDGE'){throw 'Unexpected enabled base'}
$qsf=$qsf.Replace((Join-Path $target 'core_top.v').Replace('\','/'),
                  (Join-Path $dest 'core_top_ident56.v').Replace('\','/'))
$qsf=[regex]::Replace($qsf,'(?m)^set_global_assignment -name MIF_FILE .*firmware\.mif\s*$',
 'set_global_assignment -name MIF_FILE '+(Join-Path $dest 'firmware.mif').Replace('\','/'))
$qsf=[regex]::Replace($qsf,'(?m)^set_global_assignment -name SEED \d+\s*$',
 "set_global_assignment -name SEED $Seed`n")
$qsf+="`nset_global_assignment -name VERILOG_MACRO INCLUDE_CARTRIDGE`n"
$qsf+="set_global_assignment -name VERILOG_MACRO CHISBURNER_GBA_IDENT56`n"
[IO.File]::WriteAllText((Join-Path $dest 'ap_core.qsf'),$qsf,$utf8)
$sources=@(Join-Path $dest 'core_top_ident56.v')
$sources+=@('core_bridge_cmd.v','apf/apf_top.v','apf/io_bridge_peripheral.v',
 '../../common/axi_periph_slave.v','chisburner_cart_gba_readonly.v',
 'chisburner_cart_lifecycle.v','core_constraints.sdc')|ForEach-Object{Join-Path $target $_}
$sources+=@((Join-Path $root 'core/cartridge_hal/identify56_policy.vh'),
 (Join-Path $root 'core/storage/pct_bridge_audit.v'),(Join-Path $dest 'firmware.mif'),
 (Join-Path $dest 'apf/build_id.mif'),(Join-Path $dest 'ap_core.qsf'))
[IO.File]::WriteAllLines((Join-Path $dest 'SOURCE_SHA256.txt'),[string[]]@(
 $sources|ForEach-Object{"$((Get-FileHash -LiteralPath $_).Hash.ToLowerInvariant())  $([IO.Path]::GetFullPath($_))"}),$utf8)
Push-Location $dest
try{
 & (Join-Path $QuartusBin 'quartus_sh.exe') --flow compile ap_core *> compile.log
 if($LASTEXITCODE -ne 0){throw "FPGA compile failed $dest/compile.log"}
 & (Join-Path $QuartusBin 'quartus_cdb.exe') ap_core --update_mif *> mif-update.log
 if($LASTEXITCODE -ne 0){throw 'MIF update failed'}
 & (Join-Path $QuartusBin 'quartus_asm.exe') ap_core *> assembler.log
 if($LASTEXITCODE -ne 0){throw 'Assembler failed'}
}finally{Pop-Location}
$sta=Get-Content -Raw (Join-Path $dest 'output_files/ap_core.sta.rpt')
foreach($kind in @('setup','hold')){
 $m=[regex]::Matches($sta,"Worst-case $kind slack is\s+(-?\d+(?:\.\d+)?)")
 if(!$m.Count){throw "Missing $kind"}
 $slack=($m|ForEach-Object{[double]$_.Groups[1].Value}|Measure-Object -Minimum).Minimum
 if($slack -le 0){throw "Timing failed $kind $slack"}
 "$kind slack $slack ns"
}
"Identify56 FPGA ready: $dest"
