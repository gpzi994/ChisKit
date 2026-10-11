$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$source=Get-Content (Join-Path $root '.integration/openfpgaos/src/firmware/os/targets/pocket/file.c') -Raw
$a=$source.IndexOf('static int ds_issue_command(uint32_t cmd)')
$b=$source.IndexOf('/* Serialize against the async machinery.', $a)
if($a -lt 0 -or $b -le $a){throw 'Production command helper extraction markers changed'}
New-Item -ItemType Directory -Force (Join-Path $root 'build') | Out-Null
[IO.File]::WriteAllText((Join-Path $root 'build/ds-issue-production103.inc'),$source.Substring($a,$b-$a),[Text.UTF8Encoding]::new($false))
& wsl.exe -d Ubuntu-24.04 -- bash -lc 'cd /mnt/c/ONS_FPGA/work/ChisBurner && gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined tests/test-ds-issue-stale103.c -o build/ds-issue-stale103 && build/ds-issue-stale103'
if($LASTEXITCODE){throw 'Production stale acceptance reproduction failed'}
