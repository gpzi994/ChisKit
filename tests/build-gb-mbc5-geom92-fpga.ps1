[CmdletBinding()]
param(
    [string]$QuartusBin='C:/altera_lite/25.1std/quartus/bin64',
    [int]$Seed=2,
    [string]$BaseBuild='chisburner-program65-s1-route-maxfit-buf51281m10k-win',
    [string]$OsBuild='gb-mbc5-ident91-pocket-release'
)
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$target=Join-Path $root '.integration/openfpgaos/src/fpga/targets/pocket'
$base=Join-Path (Join-Path $target 'bld') $BaseBuild
$dest=Join-Path $target "bld/chisburner-gb-mbc5-geom92-s$Seed-win"
$os=Join-Path $root ".integration/openfpgaos/src/firmware/os/bld/$OsBuild"
$utf8=[Text.UTF8Encoding]::new($false)
if(Test-Path -LiteralPath $dest){throw "Preserve existing FPGA build: $dest"}
foreach($p in @($base,(Join-Path $base 'ap_core.qsf'),(Join-Path $base 'ap_core.qpf'),
                  (Join-Path $base 'apf/build_id.mif'),(Join-Path $os 'firmware.mif'))){
    if(!(Test-Path -LiteralPath $p)){throw "Missing FPGA input: $p"}
}
New-Item -ItemType Directory -Path (Join-Path $dest 'apf') -Force|Out-Null
Copy-Item (Join-Path $base 'ap_core.qpf') $dest
Copy-Item (Join-Path $base 'apf/build_id.mif') (Join-Path $dest 'apf/build_id.mif')
Copy-Item (Join-Path $os 'firmware.mif') (Join-Path $dest 'firmware.mif')
$qsf=Get-Content -Raw (Join-Path $base 'ap_core.qsf')
foreach($macro in @('CHISBURNER_GBA_PROGRAM65','CHISBURNER_GB_HEADER84',
 'CHISBURNER_GB_MBC5_BANK85','CHISBURNER_GB_MBC5_FULLROM86',
 'CHISBURNER_GB_MBC5_WARMUP90','CHISBURNER_GB_MBC5_IDENT91',
 'CHISBURNER_GB_MBC5_GEOM92')){
    $qsf=[regex]::Replace($qsf,"(?m)^set_global_assignment -name VERILOG_MACRO $macro\s*\r?\n?",'')
}
$qsf=[regex]::Replace($qsf,'(?m)^set_global_assignment -name MIF_FILE .*firmware\.mif\s*$',
 'set_global_assignment -name MIF_FILE '+(Join-Path $dest 'firmware.mif').Replace('\','/'))
$qsf=[regex]::Replace($qsf,'(?m)^set_global_assignment -name SEED \d+\s*$',
 "set_global_assignment -name SEED $Seed")
$engine=(Join-Path $target 'chisburner_cart_gb_mbc5_identify.v').Replace('\','/')
$qsf+="`nset_global_assignment -name VERILOG_FILE $engine`n"
$qsf+="set_global_assignment -name VERILOG_MACRO CHISBURNER_GB_MBC5_GEOM92`n"
[IO.File]::WriteAllText((Join-Path $dest 'ap_core.qsf'),$qsf,$utf8)
$sources=@((Join-Path $target 'core_top.v'),(Join-Path $target 'chisburner_cart_gb_mbc5_identify.v'),
 (Join-Path $target 'chisburner_cart_lifecycle.v'),
 (Join-Path $root '.integration/openfpgaos/src/fpga/common/axi_periph_slave.v'),
 (Join-Path $dest 'firmware.mif'),(Join-Path $dest 'apf/build_id.mif'),
 (Join-Path $dest 'ap_core.qsf'))
[IO.File]::WriteAllLines((Join-Path $dest 'GB_MBC5_GEOM92_SHA256.txt'),
 [string[]]@($sources|ForEach-Object{"$((Get-FileHash $_ -Algorithm SHA256).Hash.ToLowerInvariant())  $([IO.Path]::GetFullPath($_))"}),$utf8)
Push-Location $dest
try {
    & (Join-Path $QuartusBin 'quartus_sh.exe') --flow compile ap_core *> compile.log
    if($LASTEXITCODE){throw "FPGA compile failed: $dest/compile.log"}
    & (Join-Path $QuartusBin 'quartus_cdb.exe') ap_core --update_mif *> mif-update.log
    if($LASTEXITCODE){throw 'MIF update failed'}
    & (Join-Path $QuartusBin 'quartus_asm.exe') ap_core *> assembler.log
    if($LASTEXITCODE){throw 'Assembler failed'}
} finally {Pop-Location}
$sta=Get-Content -Raw (Join-Path $dest 'output_files/ap_core.sta.rpt')
foreach($kind in @('setup','hold')){
    $m=[regex]::Matches($sta,"Worst-case $kind slack is\s+(-?\d+(?:\.\d+)?)")
    if(!$m.Count){throw "Missing $kind timing"}
    $v=($m|ForEach-Object{[double]$_.Groups[1].Value}|Measure-Object -Minimum).Minimum
    if($v-le 0){throw "Timing failed: $kind $v ns"}; "$kind slack $v ns"
}
"MBC5 geometry92 FPGA ready: $dest"
