$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$source='C:/ONS_FPGA/references/MoriBunrnner-for-ESP32-S3'
$names=@('burner_nor_db.h','burner_nor_db_data.h','burner_nor_db_data.c','burner_nor_db_lookup.c')
$prefix="// SPDX-License-Identifier: AGPL-3.0-only`n// Adapted from MoriBunrnner-for-ESP32-S3; see PROVENANCE.md.`n"
foreach($name in $names){
 $original=[IO.File]::ReadAllText((Join-Path $source "main/burner/db/$name"))
 $expected=$prefix+$original.Replace("`r`n","`n").Replace('burner_','chisburner_').Replace('BURNER_','CHISBURNER_').Replace('"../db/chisburner_nor_db.h"','"chisburner_nor_db.h"')
 $target=Join-Path $root ('core/nor/mori/'+$name.Replace('burner_','chisburner_'))
 $actual=[IO.File]::ReadAllText($target).Replace("`r`n","`n")
 if($actual.TrimEnd() -cne $expected.TrimEnd()){throw "Source parity failed: $name"}
}
$license=[IO.File]::ReadAllText((Join-Path $source 'LICENSE')).Replace("`r`n","`n").TrimEnd()
$copy=[IO.File]::ReadAllText((Join-Path $root 'core/nor/mori/COPYING')).Replace("`r`n","`n").TrimEnd()
if($copy -cne $license){throw 'License parity failed'}
Write-Output 'PASS: all four database source files match upstream after documented namespace/include adaptation; license preserved'
