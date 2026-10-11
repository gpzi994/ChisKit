$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$wroot='/mnt/'+$root.Substring(0,1).ToLowerInvariant()+$root.Substring(2).Replace('\','/')
& wsl.exe -d Ubuntu-24.04 -- bash -lc "cd '$wroot/.integration/openfpgaos/src/firmware/os' && make -j4 TARGET=pocket BLD=bld/ui106-pocket EXTRA_CFLAGS='-DCART_MMIO_ENABLED=1 -DCART_UI98=1 -DCART_S29GL256_PROGRAMMER=1 -DCHISBURNER_OUTPUT_ENABLED=1' all"
if($LASTEXITCODE){throw 'UI98 OS build failed; do not bypass contract-check'}
. (Join-Path $PSScriptRoot 'os-image-integrity.ps1')
$null=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes((Join-Path $root '.integration/openfpgaos/src/firmware/os/bld/ui106-pocket/os.bin')))

