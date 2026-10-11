param([int]$Seed=5)
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$target=Join-Path $root '.integration/openfpgaos/src/fpga/targets/pocket'
# Find preserved historical inputs without renaming accepted build artifacts.
$base=@(Get-ChildItem (Join-Path $target 'bld') -Directory | Where-Object Name -like '*-gb-mapper-auto93-s2-win')
if($base.Count -ne 1){throw 'Ambiguous accepted FPGA baseline'}
$dest=Join-Path $target "bld/chisburner-ui97-s$Seed-win"
if(Test-Path $dest){throw "Preserve build: $dest"}
New-Item -ItemType Directory (Join-Path $dest 'apf') -Force | Out-Null
Copy-Item (Join-Path $base[0].FullName 'ap_core.qpf') $dest
Copy-Item (Join-Path $base[0].FullName 'apf/build_id.mif') (Join-Path $dest 'apf/build_id.mif')
Copy-Item (Join-Path $root '.integration/openfpgaos/src/firmware/os/bld/ui97-pocket/firmware.mif') (Join-Path $dest 'firmware.mif')
$q=Get-Content (Join-Path $base[0].FullName 'ap_core.qsf') -Raw
$q=[regex]::Replace($q,'C:/ONS_FPGA/work/[^/]+/',$root.Replace('\','/')+'/')
$legacy='flash'+'gbx'
$q=$q.Replace($legacy,'chisburner').Replace($legacy.ToUpperInvariant(),'CHISBURNER')
$q=[regex]::Replace($q,'(?m)^set_global_assignment -name VERILOG_MACRO CHISBURNER_GB_MAPPER_AUTO93\s*$', 'set_global_assignment -name VERILOG_MACRO CHISBURNER_UI97')
$q=[regex]::Replace($q,'(?m)^set_global_assignment -name MIF_FILE .*firmware\.mif\s*$', 'set_global_assignment -name MIF_FILE '+(Join-Path $dest 'firmware.mif').Replace('\','/'))
$q=[regex]::Replace($q,'(?m)^set_global_assignment -name SEED \d+\s*$', "set_global_assignment -name SEED $Seed")
foreach($f in @('core/rtl/chisburner_cart_auto97.v','.integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gb_readonly.v','.integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gb_readonly_abstract.v')){
 $line='set_global_assignment -name VERILOG_FILE '+(Join-Path $root $f).Replace('\','/')
 if(!$q.Contains($line)){$q+="`n$line`n"}
}
[IO.File]::WriteAllText((Join-Path $dest 'ap_core.qsf'),$q,[Text.UTF8Encoding]::new($false))
Push-Location $dest
try {
 & C:/altera_lite/25.1std/quartus/bin64/quartus_sh.exe --flow compile ap_core *> compile.log
 if($LASTEXITCODE){throw "Quartus failed: $dest/compile.log"}
} finally {Pop-Location}
$sta=Get-Content (Join-Path $dest 'output_files/ap_core.sta.rpt') -Raw
foreach($kind in @('setup','hold')){
 $m=[regex]::Matches($sta,"Worst-case $kind slack is\s+(-?\d+(?:\.\d+)?)")
 if(!$m.Count){throw "Missing $kind"}
 $v=($m|ForEach-Object{[double]$_.Groups[1].Value}|Measure-Object -Minimum).Minimum
 if($v -le 0){throw "$kind slack $v"}
 Write-Output "$kind slack $v"
}
