[CmdletBinding()]
param(
    [string]$Distro='Ubuntu-24.04',
    [string]$BuildDir='bld/program66-pocket-release'
)

$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$osRoot=Join-Path $root '.integration/openfpgaos/src/firmware/os'
$dest=Join-Path $osRoot $BuildDir
$wroot='/mnt/'+$root.Substring(0,1).ToLowerInvariant()+'/'+$root.Substring(3).Replace('\','/')
$wos="$wroot/.integration/openfpgaos/src/firmware/os"
$utf8=[Text.UTF8Encoding]::new($false)

if(Test-Path -LiteralPath $dest){throw "Preserve existing PROGRAM65 OS build: $dest"}

$flags=@(
    '-DCART_MMIO_ENABLED=1',
    '-DCART_S29GL256_PROGRAMMER=1',
    '-DCHISBURNER_OUTPUT_ENABLED=1'
) -join ' '
$command="cd '$wos' && make -j 4 TARGET=pocket BLD='$BuildDir' EXTRA_CFLAGS='$flags' all"
& wsl.exe -d $Distro -- bash -lc $command
if($LASTEXITCODE -ne 0){throw 'PROGRAM65 OS build failed'}

foreach($name in @('os.bin','firmware.mif','firmware.elf','firmware.map')){
    if(!(Test-Path -LiteralPath (Join-Path $dest $name))){throw "Missing OS output $name"}
}

$sources=@(
    (Join-Path $osRoot 'Makefile'),
    (Join-Path $osRoot 'hal/regs.h'),
    (Join-Path $osRoot 'kernel/services_table.c'),
    (Join-Path $root '.integration/openfpgaos/src/firmware/api/of_cart.h'),
    (Join-Path $root '.integration/openfpgaos/src/firmware/api/of_services.h')
)
$manifest=@("EXTRA_CFLAGS=$flags")
$manifest += $sources|ForEach-Object{
    "$((Get-FileHash -LiteralPath $_ -Algorithm SHA256).Hash.ToLowerInvariant())  $([IO.Path]::GetFullPath($_))"
}
[IO.File]::WriteAllLines((Join-Path $dest 'PROGRAM65_OS_SHA256.txt'),
                         [string[]]$manifest,$utf8)
"Program65 OS ready: $dest"
