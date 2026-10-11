[CmdletBinding()]
param(
    [string]$QuartusBin='C:/altera_lite/25.1std/quartus/bin64',
    [int]$Seed=2,
    [string]$BaseBuild='chisburner-program65-s1-route-maxfit-buf51281m10k-win',
    [string]$OsBuild='gb-mbc5-fullrom86-pocket-release-r1'
)

$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$target=Join-Path $root '.integration/openfpgaos/src/fpga/targets/pocket'
$base=Join-Path (Join-Path $target 'bld') $BaseBuild
$dest=Join-Path $target "bld/chisburner-gb-mbc5-fullrom86-s$Seed-win"
$os=Join-Path $root ".integration/openfpgaos/src/firmware/os/bld/$OsBuild"
$utf8=[Text.UTF8Encoding]::new($false)

if(Test-Path -LiteralPath $dest){throw "Preserve existing FPGA build: $dest"}
foreach($path in @($base,(Join-Path $base 'ap_core.qsf'),(Join-Path $base 'ap_core.qpf'),
                   (Join-Path $base 'apf/build_id.mif'),(Join-Path $os 'firmware.mif'))){
    if(!(Test-Path -LiteralPath $path)){throw "Missing FPGA build input: $path"}
}

New-Item -ItemType Directory -Path (Join-Path $dest 'apf') -Force | Out-Null
Copy-Item -LiteralPath (Join-Path $base 'ap_core.qpf') -Destination $dest
Copy-Item -LiteralPath (Join-Path $base 'apf/build_id.mif') -Destination (Join-Path $dest 'apf/build_id.mif')
Copy-Item -LiteralPath (Join-Path $os 'firmware.mif') -Destination (Join-Path $dest 'firmware.mif')

$qsf=Get-Content -Raw -LiteralPath (Join-Path $base 'ap_core.qsf')
foreach($macro in @('CHISBURNER_GBA_PROGRAM65','CHISBURNER_GB_HEADER84',
                    'CHISBURNER_GB_MBC5_BANK85','CHISBURNER_GB_MBC5_FULLROM86')){
    $qsf=[regex]::Replace($qsf,"(?m)^set_global_assignment -name VERILOG_MACRO $macro\s*\r?\n?",'')
}
$qsf=[regex]::Replace($qsf,'(?m)^set_global_assignment -name MIF_FILE .*firmware\.mif\s*$',
    'set_global_assignment -name MIF_FILE '+(Join-Path $dest 'firmware.mif').Replace('\','/'))
$qsf=[regex]::Replace($qsf,'(?m)^set_global_assignment -name SEED \d+\s*$',
    "set_global_assignment -name SEED $Seed")
$engine=(Join-Path $target 'chisburner_cart_gb_mbc5_fullrom.v').Replace('\','/')
$qsf+="`nset_global_assignment -name VERILOG_FILE $engine`n"
$qsf+="set_global_assignment -name VERILOG_MACRO CHISBURNER_GB_MBC5_FULLROM86`n"
[IO.File]::WriteAllText((Join-Path $dest 'ap_core.qsf'),$qsf,$utf8)

$sources=@(
    (Join-Path $target 'core_top.v'),
    (Join-Path $target 'chisburner_cart_gb_mbc5_fullrom.v'),
    (Join-Path $target 'chisburner_cart_lifecycle.v'),
    (Join-Path $root '.integration/openfpgaos/src/fpga/common/axi_periph_slave.v'),
    (Join-Path $dest 'firmware.mif'),
    (Join-Path $dest 'apf/build_id.mif'),
    (Join-Path $dest 'ap_core.qsf')
)
[IO.File]::WriteAllLines((Join-Path $dest 'GB_MBC5_FULLROM86_SHA256.txt'),
    [string[]]@($sources|ForEach-Object{
        "$((Get-FileHash -LiteralPath $_ -Algorithm SHA256).Hash.ToLowerInvariant())  $([IO.Path]::GetFullPath($_))"
    }),$utf8)

Push-Location $dest
try {
    & (Join-Path $QuartusBin 'quartus_sh.exe') --flow compile ap_core *> compile.log
    if($LASTEXITCODE -ne 0){throw "FPGA compile failed: $dest/compile.log"}
    & (Join-Path $QuartusBin 'quartus_cdb.exe') ap_core --update_mif *> mif-update.log
    if($LASTEXITCODE -ne 0){throw "MIF update failed: $dest/mif-update.log"}
    & (Join-Path $QuartusBin 'quartus_asm.exe') ap_core *> assembler.log
    if($LASTEXITCODE -ne 0){throw "Assembler failed: $dest/assembler.log"}
} finally { Pop-Location }

$sta=Get-Content -Raw -LiteralPath (Join-Path $dest 'output_files/ap_core.sta.rpt')
foreach($kind in @('setup','hold')){
    $matches=[regex]::Matches($sta,"Worst-case $kind slack is\s+(-?\d+(?:\.\d+)?)")
    if(!$matches.Count){throw "Missing $kind timing result"}
    $slack=($matches|ForEach-Object{[double]$_.Groups[1].Value}|Measure-Object -Minimum).Minimum
    if($slack -le 0){throw "Timing failed: $kind slack $slack ns"}
    "$kind slack $slack ns"
}
$qsfFinal=Get-Content -Raw -LiteralPath (Join-Path $dest 'ap_core.qsf')
if($qsfFinal -notmatch '(?m)^set_global_assignment -name VERILOG_MACRO CHISBURNER_GB_MBC5_FULLROM86\s*$' -or
   $qsfFinal -match '(?m)^set_global_assignment -name VERILOG_MACRO CHISBURNER_GBA_PROGRAM65\s*$'){
    throw 'Final FPGA profile macro set is unsafe'
}
"MBC5 fullrom86 FPGA ready: $dest"
