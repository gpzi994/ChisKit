param(
 [int]$Seed=10,
 [ValidateSet('standard','closure','floorplan','floorplan5','floorplan5control','phys','retime','control')][string]$Variant='standard'
)
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$suffix=if($Variant -eq 'standard'){''}else{"-$Variant"}
$dir=Join-Path $root ".integration/openfpgaos/src/fpga/targets/pocket/bld/chisburner-ui107-s$Seed$suffix-win"
# Redirected stdout is created when Quartus starts; compile.log is assembled
# only after Quartus exits and cannot prove inputs predate the compilation.
$started=(Get-Item (Join-Path $dir 'compile.stdout.log')).CreationTimeUtc
$paths=@()
foreach($line in Get-Content (Join-Path $dir 'ap_core.qsf')){
 if($line -match '^set_global_assignment -name (?:VERILOG_FILE|SYSTEMVERILOG_FILE|VHDL_FILE|SDC_FILE|MIF_FILE) (.+)$'){
  $p=$Matches[1].Trim('"');if(![IO.Path]::IsPathRooted($p)){$p=Join-Path $dir $p}
  $p=[IO.Path]::GetFullPath($p)
  if((Get-Item -LiteralPath $p).LastWriteTimeUtc -gt $started){throw "Input newer than compile: $p"}
  $paths+=$p
 }
}
if(!$paths.Count){throw 'No FPGA inputs found'}
$receipt=Join-Path $dir 'UI98_INPUTS_SHA256.txt'
$lines=@($paths|Sort-Object -Unique|ForEach-Object{"$((Get-FileHash -LiteralPath $_ -Algorithm SHA256).Hash.ToLowerInvariant())  $_"})
if(Test-Path $receipt){
 if((Get-Content $receipt -Raw).TrimEnd() -cne ($lines -join "`n").TrimEnd()){throw 'Captured inputs changed'}
}else{[IO.File]::WriteAllText($receipt,($lines -join "`n")+"`n",[Text.UTF8Encoding]::new($false))}
Write-Output "FPGA inputs checked: $($paths.Count); unchanged since compile start"

