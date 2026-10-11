$ErrorActionPreference='Stop'
$root=Join-Path $PSScriptRoot '..'
$src=Get-Content "$root/.integration/openfpgaos/src/firmware/os/kernel/main.c" -Raw
$start=$src.IndexOf('stage = "output create";')
$end=$src.IndexOf('stage = "PSRAM0 audit";', $start)
if($start -lt 0 -or $end -lt $start){throw 'Missing diagnostic stages'}
$body=$src.Substring($start,$end-$start)
if($body.Contains('of_file_output_flush(')){throw 'Unsupported target FLUSH reintroduced'}
foreach($s in @('uint8_t payload[1024]','of_file_output_write(','of_file_read(OUTPUT_SLOT','scratch[i] != psram128_pattern(i, 0xc1u)','Output compare 1024 PASS')){
 if(!$body.Contains($s)){throw "Missing regression guard: $s"}
}
Write-Output 'PASS: output payload isolated, no FLUSH, independent 1024-byte readback comparison (source checks only).'
