$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$source=Get-Content (Join-Path $root '.integration/openfpgaos/src/firmware/os/targets/pocket/file.c') -Raw
$services=Get-Content (Join-Path $root '.integration/openfpgaos/src/firmware/os/kernel/services_table.c') -Raw
if($services -notmatch '#define\s+CHISBURNER_OUTPUT_SLOT\s+5u'){
 throw 'ChisBurner output service must use dedicated data slot 5'
}
$a=$source.IndexOf('static int file_wait_complete_mask(')
$b=$source.IndexOf('static int file_wait_complete(void)', $a)
$c=$source.IndexOf('static uint32_t output_open_slot;')
$d=$source.IndexOf('int of_file_output_flush(', $c)
if($a -lt 0 -or $b -le $a -or $c -lt 0 -or $d -le $c){throw 'OS test extraction markers changed'}
$body=$source.Substring($a,$b-$a)+$source.Substring($c,$d-$c)
# Only replace the RISC-V fence instruction for the native host build.
$body=$body.Replace('__asm__ volatile("fence" ::: "memory");','fence();')
[IO.File]::WriteAllText((Join-Path $root 'build/ui98-output-body.inc'),$body,[Text.UTF8Encoding]::new($false))
& wsl.exe -d Ubuntu-24.04 -- bash -lc 'cd /mnt/c/ONS_FPGA/work/ChisBurner && g++ -std=c++17 -Wall -Wextra -Werror -fsanitize=address,undefined -I.integration/openfpgaos/src/firmware/api tests/test-ui98-output.cpp -o build/ui98-output-host && build/ui98-output-host'
if($LASTEXITCODE){throw 'Output backend test failed'}
