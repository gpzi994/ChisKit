param([ValidateSet(108,109,110,111)][int]$Revision=108)
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$wroot='/mnt/'+$root.Substring(0,1).ToLowerInvariant()+$root.Substring(2).Replace('\','/')
$build=if($Revision -ge 110){"ui$Revision-release-pocket"}elseif($Revision -eq 109){'ui109-board-pocket'}else{'ui108-pocket'}
$extra=if($Revision -ge 110){' -DCART_NOR109=1 -DCART_NOR110=1'}elseif($Revision -eq 109){' -DCART_NOR109=1'}else{''}
$inputs=@(Get-ChildItem "$root/.integration/openfpgaos/src/firmware/os","$root/.integration/openfpgaos/src/firmware/api","$root/core/nor" -Recurse -File | Where-Object {$_.FullName -notmatch '[/\\]bld[/\\]' -and ($_.Extension -in '.c','.h','.S','.s','.ld','.mk' -or $_.Name -eq 'Makefile')} | Sort-Object FullName)
$before=@($inputs | ForEach-Object {"$((Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash.ToLowerInvariant())  $($_.FullName)"})
& wsl.exe -d Ubuntu-24.04 -- bash -lc "cd '$wroot/.integration/openfpgaos/src/firmware/os' && make -j4 TARGET=pocket BLD=bld/$build EXTRA_CFLAGS='-DCART_MMIO_ENABLED=1 -DCART_UI98=1 -DCART_NOR108=1 -DCART_S29GL256_PROGRAMMER=1 -DCHISBURNER_OUTPUT_ENABLED=1$extra' all"
if($LASTEXITCODE){throw 'NOR108 OS build failed; do not bypass contract-check'}
. (Join-Path $PSScriptRoot 'os-image-integrity.ps1')
$null=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes((Join-Path $root ".integration/openfpgaos/src/firmware/os/bld/$build/os.bin")))
$after=@($inputs | ForEach-Object {"$((Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash.ToLowerInvariant())  $($_.FullName)"})
if(($before -join "`n") -cne ($after -join "`n")){throw 'OS inputs changed during build'}
[IO.File]::WriteAllLines("$root/.integration/openfpgaos/src/firmware/os/bld/$build/OS_INPUTS_SHA256.txt",[string[]]$after,[Text.UTF8Encoding]::new($false))
