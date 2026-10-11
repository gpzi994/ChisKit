param([int]$Seed=4,[string]$Variant='floorplan5control')
$ErrorActionPreference='Stop'
& (Join-Path $PSScriptRoot 'build-ui108-package.ps1') -Revision 109 -Seed $Seed -Variant $Variant
