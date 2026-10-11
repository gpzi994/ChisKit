$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$wroot='/mnt/'+$root.Substring(0,1).ToLowerInvariant()+$root.Substring(2).Replace('\','/')
& wsl.exe -d Ubuntu-24.04 -- bash -lc "cd '$wroot/.integration/openfpgaos/src/firmware/os' && make -j4 TARGET=pocket BLD=bld/ui97-pocket EXTRA_CFLAGS='-DCART_MMIO_ENABLED=1' all"
if($LASTEXITCODE){throw 'UI97 OS build failed; do not bypass contract-check'}
. (Join-Path $PSScriptRoot 'os-image-integrity.ps1')
$null=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes((Join-Path $root '.integration/openfpgaos/src/firmware/os/bld/ui97-pocket/os.bin')))
