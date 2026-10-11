$ErrorActionPreference = 'Stop'
$root = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$top = Get-Content -LiteralPath "$root/.integration/openfpgaos/src/fpga/targets/pocket/core_top.v" -Raw

foreach ($required in @(
    "reg target_dataslot_getfile_cnt_1 = 1'b0;",
    'target_dataslot_getfile_cnt_1 <= target_dataslot_getfile;',
    '(target_dataslot_getfile && !target_dataslot_getfile_cnt_1)',
    'target_buffer_resp_struct <= cpu_ds_resp_sync2;'
)) {
    if (-not $top.Contains($required)) {
        throw "Missing GETFILE response-address CDC wiring: $required"
    }
}

$latchStart = $top.IndexOf('if ((target_dataslot_read && !target_dataslot_read_1)')
$latchEnd = $top.IndexOf('// ============================================================', $latchStart)
if ($latchStart -lt 0 -or $latchEnd -lt 0) {
    throw 'Could not isolate data-slot parameter latch block'
}
$latch = $top.Substring($latchStart, $latchEnd - $latchStart)
foreach ($required in @(
    '(target_dataslot_getfile && !target_dataslot_getfile_cnt_1)',
    'target_dataslot_id <= cpu_ds_id_sync2;',
    'target_buffer_resp_struct <= cpu_ds_resp_sync2;'
)) {
    if (-not $latch.Contains($required)) {
        throw "GETFILE edge does not share the complete command-parameter latch: $required"
    }
}

Write-Output 'PSRAM128 GETFILE response-address CDC wiring PASS'
