param(
 [int]$Seed=5,
 [ValidateSet('standard','closure','floorplan','floorplan5','floorplan5control','phys','retime','control')][string]$Variant='standard'
)
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$target=Join-Path $root '.integration/openfpgaos/src/fpga/targets/pocket'
# Find preserved historical inputs without renaming accepted build artifacts.
$base=@(Get-ChildItem (Join-Path $target 'bld') -Directory | Where-Object Name -like '*-gb-mapper-auto93-s2-win')
if($base.Count -ne 1){throw 'Ambiguous accepted FPGA baseline'}
$suffix=if($Variant -eq 'standard'){''}else{"-$Variant"}
$dest=Join-Path $target "bld/chisburner-ui107-s$Seed$suffix-win"
if(Test-Path $dest){throw "Preserve build: $dest"}
New-Item -ItemType Directory (Join-Path $dest 'apf') -Force | Out-Null
Copy-Item (Join-Path $base[0].FullName 'ap_core.qpf') $dest
Copy-Item (Join-Path $base[0].FullName 'apf/build_id.mif') (Join-Path $dest 'apf/build_id.mif')
Copy-Item (Join-Path $root '.integration/openfpgaos/src/firmware/os/bld/ui107-pocket/firmware.mif') (Join-Path $dest 'firmware.mif')
$q=Get-Content (Join-Path $base[0].FullName 'ap_core.qsf') -Raw
$q=[regex]::Replace($q,'C:/ONS_FPGA/work/[^/]+/',$root.Replace('\','/')+'/')
$legacy='flash'+'gbx'
$q=$q.Replace($legacy,'chisburner').Replace($legacy.ToUpperInvariant(),'CHISBURNER')
$q=[regex]::Replace($q,'(?m)^set_global_assignment -name VERILOG_MACRO CHISBURNER_GB_MAPPER_AUTO93\s*$', 'set_global_assignment -name VERILOG_MACRO CHISBURNER_UI98')
$q=[regex]::Replace($q,'(?m)^set_global_assignment -name MIF_FILE .*firmware\.mif\s*$', 'set_global_assignment -name MIF_FILE '+(Join-Path $dest 'firmware.mif').Replace('\','/'))
$q=[regex]::Replace($q,'(?m)^set_global_assignment -name SEED \d+\s*$', "set_global_assignment -name SEED $Seed")
if($Variant -eq 'control' -or $Variant -eq 'floorplan5control'){
 $q=$q.Replace('PHYSICAL_SYNTHESIS_COMBO_LOGIC OFF','PHYSICAL_SYNTHESIS_COMBO_LOGIC ON')
 $q=$q.Replace('PHYSICAL_SYNTHESIS_EFFORT NORMAL','PHYSICAL_SYNTHESIS_EFFORT EXTRA')
}
if($Variant -eq 'floorplan5control'){
 $q+="`nset_global_assignment -name OPTIMIZE_HOLD_TIMING ALL_PATHS`n"
 $q+="set_global_assignment -name OPTIMIZE_MULTI_CORNER_TIMING ON`n"
}
if($Variant -eq 'phys'){
 $q=$q.Replace('PHYSICAL_SYNTHESIS_EFFORT NORMAL','PHYSICAL_SYNTHESIS_EFFORT EXTRA')
}
if($Variant -eq 'retime'){
 $q=$q.Replace('PHYSICAL_SYNTHESIS_REGISTER_RETIMING OFF','PHYSICAL_SYNTHESIS_REGISTER_RETIMING ON')
 $q=$q.Replace('PHYSICAL_SYNTHESIS_EFFORT NORMAL','PHYSICAL_SYNTHESIS_EFFORT EXTRA')
}
if($Variant -eq 'floorplan' -or $Variant -eq 'floorplan5' -or $Variant -eq 'floorplan5control'){
 # The accepted GBA image places the GPU texture multiplier at Y27 and the
 # scanout multiplier at Y37.  Keeping this proven DSP ordering prevents the
 # nine-level sp_t -> p0a_t_y cone from being stretched across the fabric as
 # UI engines are added.  This is placement-only; no clock or path exception.
 $q+="`n"+'set_location_assignment DSP_X33_Y27_N0 -to "core_top:ic|gpu_core:gpu|Mult16~8"'+"`n"
 $q+='set_location_assignment DSP_X33_Y37_N0 -to "core_top:ic|video_CRT_scanout_indexed_BRAM:scanout|Mult0~mac"'+"`n"
}
if($Variant -eq 'floorplan5' -or $Variant -eq 'floorplan5control'){
 # Quartus DSE's documented high-effort point for Cyclone V.  Keep seed and
 # netlist fixed while giving the placer more optimization time for the CPU.
 $q=$q.Replace('PLACEMENT_EFFORT_MULTIPLIER 3.0','PLACEMENT_EFFORT_MULTIPLIER 5.0')
}
foreach($f in @('core/rtl/chisburner_cart_auto98.v','.integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gba_s29gl256.v','.integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gb_mbc5_identify.v','.integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gb_readonly.v','.integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gb_readonly_abstract.v')){
 $line='set_global_assignment -name VERILOG_FILE '+(Join-Path $root $f).Replace('\','/')
 if(!$q.Contains($line)){$q+="`n$line`n"}
}
[IO.File]::WriteAllText((Join-Path $dest 'ap_core.qsf'),$q,[Text.UTF8Encoding]::new($false))
Push-Location $dest
try {
 $stdout=Join-Path $dest 'compile.stdout.log'
 $stderr=Join-Path $dest 'compile.stderr.log'
 $proc=Start-Process -FilePath 'C:/altera_lite/25.1std/quartus/bin64/quartus_sh.exe' `
  -ArgumentList @('--flow','compile','ap_core') -RedirectStandardOutput $stdout `
  -RedirectStandardError $stderr -WindowStyle Hidden -Wait -PassThru
 [IO.File]::WriteAllText((Join-Path $dest 'compile.log'),
  ([IO.File]::ReadAllText($stdout)+[IO.File]::ReadAllText($stderr)),
  [Text.UTF8Encoding]::new($false))
 if($proc.ExitCode){throw "Quartus failed: $dest/compile.log"}
} finally {Pop-Location}
$sta=Get-Content (Join-Path $dest 'output_files/ap_core.sta.rpt') -Raw
foreach($kind in @('setup','hold')){
 $m=[regex]::Matches($sta,"Worst-case $kind slack is\s+(-?\d+(?:\.\d+)?)")
 if(!$m.Count){throw "Missing $kind"}
 $v=($m|ForEach-Object{[double]$_.Groups[1].Value}|Measure-Object -Minimum).Minimum
 if($v -le 0){throw "$kind slack $v"}
 Write-Output "$kind slack $v"
}
& (Join-Path $PSScriptRoot 'check-ui107-fpga-inputs.ps1') -Seed $Seed -Variant $Variant


