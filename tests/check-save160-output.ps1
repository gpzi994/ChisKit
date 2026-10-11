$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$src=Get-Content "$root/.integration/openfpgaos/src/firmware/os/targets/pocket/file.c" -Raw
$a=$src.IndexOf('static uint32_t output_open_slot;')
$b=$src.IndexOf('int of_file_output_flush(', $a)
if($a -lt 0 -or $b -le $a){throw 'Extraction markers changed'}
# Host pointers are wider than the 32-bit Pocket address registers.
$body=$src.Substring($a,$b-$a).Replace('uint32_t param_addr =','uintptr_t param_addr =').Replace('uint32_t scratch =','uintptr_t scratch =')
[IO.File]::WriteAllText("$root/build/save160-output-body.inc",$body)
& wsl.exe -d Ubuntu-24.04 -- bash -lc 'cd /mnt/c/ONS_FPGA/work/ChisBurner && g++ -std=c++17 -Wall -Wextra -Werror -fsanitize=undefined -I.integration/openfpgaos/src/firmware/api tests/test-save160-output.cpp -o build/save160-output-test && build/save160-output-test'
if($LASTEXITCODE){throw 'Backup output regression failed'}
