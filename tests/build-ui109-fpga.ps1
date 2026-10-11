param(
 [int]$Seed=4,
 [ValidateSet('standard','closure','floorplan','floorplan5','floorplan5control','phys','retime','control')]
 [string]$Variant='floorplan5control'
)
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
. (Join-Path $PSScriptRoot 'os-image-integrity.ps1')
$os=Join-Path $root '.integration/openfpgaos/src/firmware/os/bld/ui109-board-pocket/os.bin'
$integrity=Get-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes($os))
if(!$integrity.CrcMatches){throw 'OS109 internal CRC mismatch'}
& (Join-Path $PSScriptRoot 'build-ui108-fpga.ps1') -Revision 109 -Seed $Seed -Variant $Variant
