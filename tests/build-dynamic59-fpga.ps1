[CmdletBinding()]
param([string]$QuartusBin='C:/altera_lite/25.1std/quartus/bin64',[int]$Seed=7)
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$target=Join-Path $root '.integration/openfpgaos/src/fpga/targets/pocket'
$base=Join-Path $target 'bld/chisburner-storage-trace-s44-win'
$dest=Join-Path $target "bld/chisburner-dynamic59-s$Seed-win"
if(Test-Path -LiteralPath $dest){throw 'Preserve existing FPGA build'}
$utf8=[Text.UTF8Encoding]::new($false)
New-Item -ItemType Directory -Path (Join-Path $dest 'apf') -Force|Out-Null
foreach($f in @('firmware.mif','ap_core.qpf','apf/build_id.mif')){
 Copy-Item -LiteralPath (Join-Path $base $f) -Destination (Join-Path $dest $f)
}
$qsf=Get-Content -Raw (Join-Path $base 'ap_core.qsf')
if($qsf-match '(?m)^set_global_assignment -name VERILOG_MACRO INCLUDE_CARTRIDGE\s*$'){throw 'Cartridge profile forbidden'}
$mif=(Join-Path $dest 'firmware.mif').Replace('\','/')
$rtl=(Join-Path $root 'core/storage/pct_bridge_audit.v').Replace('\','/')
$qsf=[regex]::Replace($qsf,'(?m)^set_global_assignment -name MIF_FILE .*firmware\.mif\s*$',"set_global_assignment -name MIF_FILE $mif")
$qsf=[regex]::Replace($qsf,'(?m)^set_global_assignment -name SEED \d+\s*$',"set_global_assignment -name SEED $Seed")
$qsf+="`nset_global_assignment -name VERILOG_MACRO CHISBURNER_STORAGE_EVENTS`n"
$qsf+="set_global_assignment -name VERILOG_FILE $rtl`n"
$qsf+="set_global_assignment -name VERILOG_MACRO CHISBURNER_STORAGE_AUDIT`n"
[IO.File]::WriteAllText((Join-Path $dest 'ap_core.qsf'),$qsf,$utf8)
$sources=@('core_top.v','core_bridge_cmd.v','apf/apf_top.v','apf/io_bridge_peripheral.v','../../common/axi_periph_slave.v','core_constraints.sdc')|ForEach-Object{Join-Path $target $_}
$sources+=@($rtl,(Join-Path $dest 'firmware.mif'),(Join-Path $dest 'apf/build_id.mif'),(Join-Path $dest 'ap_core.qsf'))
[IO.File]::WriteAllLines((Join-Path $dest 'SOURCE_SHA256.txt'),[string[]]@($sources|ForEach-Object{"$((Get-FileHash -LiteralPath $_).Hash.ToLowerInvariant())  $([IO.Path]::GetFullPath($_))"}),$utf8)
Push-Location $dest
try{
 & (Join-Path $QuartusBin 'quartus_sh.exe') --flow compile ap_core *> compile.log
 if($LASTEXITCODE-ne 0){throw "FPGA compile failed $dest/compile.log"}
 & (Join-Path $QuartusBin 'quartus_cdb.exe') ap_core --update_mif *> mif-update.log
 if($LASTEXITCODE-ne 0){throw 'MIF update failed'}
 & (Join-Path $QuartusBin 'quartus_asm.exe') ap_core *> assembler.log
 if($LASTEXITCODE-ne 0){throw 'Assembler failed'}
}finally{Pop-Location}
$sta=Get-Content -Raw (Join-Path $dest 'output_files/ap_core.sta.rpt')
foreach($kind in @('setup','hold')){
 $m=[regex]::Matches($sta,"Worst-case $kind slack is\s+(-?\d+(?:\.\d+)?)")
 if(!$m.Count){throw "Missing $kind"}
 $slack=($m|ForEach-Object{[double]$_.Groups[1].Value}|Measure-Object -Minimum).Minimum
 if($slack-le 0){throw "Timing failed $kind $slack"}
 "$kind slack $slack ns"
}
"Dynamic59 FPGA ready: $dest"
