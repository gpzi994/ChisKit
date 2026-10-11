$ErrorActionPreference = 'Stop'
$root = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$bridge = Join-Path $root '.integration/openfpgaos/src/fpga/targets/pocket/core_bridge_cmd.v'
$text = [IO.File]::ReadAllText($bridge)

if ($text -notmatch 'else if\s*\(target_dataslot_flush_queue\)\s*begin') {
    throw 'FLUSH launch is missing or still gated by SDRAM admission'
}
if ($text -match 'target_dataslot_flush_queue\s*&&\s*storage_can_launch') {
    throw 'FLUSH must not depend on the SDRAM payload admission gate'
}
if ($text -notmatch 'target_dataslot_(read|write|openfile|getfile)_queue\s*&&\s*storage_can_launch') {
    throw 'Payload commands unexpectedly lost their SDRAM admission gate'
}

Write-Host 'PASS: payload-less FLUSH bypasses SDRAM admission; payload commands remain gated.'
