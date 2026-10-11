[CmdletBinding()]
param(
    [string]$Distro='Ubuntu-24.04',
    [string]$OpenFpgaSdk='/home/lin13/work/ONS_FPGA/openfpgaSDK'
)

$ErrorActionPreference='Stop'
. (Join-Path $PSScriptRoot 'os-image-integrity.ps1')

function Sha([string]$Path) {
    (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant()
}
function Wsl([string]$Command) {
    & wsl.exe -d $Distro -- bash -lc $Command
    if ($LASTEXITCODE -ne 0) { throw "Build/test failed: $Command" }
}

$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$wroot='/mnt/'+$root.Substring(0,1).ToLowerInvariant()+'/'+$root.Substring(3).Replace('\','/')
$baseBundle=Join-Path $root 'build/smoke/ChisBurner-smoke.78-b1-reset-readonly-diag'
$base=Join-Path $baseBundle 'SD_ROOT'
$bundle=Join-Path $root 'build/smoke/ChisBurner-smoke.79b-playcart-runtime-rom-readonly'
$zip="$bundle.zip"
$app=Join-Path $root 'build/program79-app/app.elf'
$readme=Join-Path $root 'app/gba_program79_launchdiag/README_TEST79_CN.txt'
$utf8=[Text.UTF8Encoding]::new($false)
$expectedOsHash='cd1e40641a1d1d211a4681adb83c2d4bde392102b8e5453366d70aa40a0876a3'
$expectedRbfHash='aca0beaa8c9351bb65117c4b1e74a959129b74e15898f382ae2154385c8a242b'

if ((Test-Path -LiteralPath $bundle) -or (Test-Path -LiteralPath $zip)) {
    throw 'Preserve existing smoke79 package'
}
foreach ($path in @($base,$readme)) {
    if (!(Test-Path -LiteralPath $path)) { throw "Missing package input $path" }
}

$source=Get-Content -Raw -LiteralPath (Join-Path $root 'app/gba_program79_launchdiag/main.c')
foreach ($forbiddenCall in @('of_cart_flash_arm','of_cart_flash_erase_sector',
                              'of_cart_flash_program','of_output_write')) {
    if ($source.Contains($forbiddenCall)) {
        throw "Read-only diagnostic contains forbidden call: $forbiddenCall"
    }
}

Wsl "cd '$wroot/app/gba_program79_launchdiag' && make -B host-test all OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0"
Wsl "cd '$wroot' && cc -DOF_PC -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined -fno-omit-frame-pointer -I.integration/openfpgaos/src/firmware/api app/gba_program79_launchdiag/main.c -o build/program79-app/pc-ui-san && build/program79-app/pc-ui-san >/dev/null"

$elf=[IO.File]::ReadAllBytes($app)
$elfText=[Text.Encoding]::ASCII.GetString($elf)
foreach ($requiredText in @('smoke.79b','QUIT; START WITH PLAY CARTRIDGE',
                             'LOAD PROGRAM ROM (SLOT 4)',
                             'NO ARM/ERASE/PROGRAM CALLS EXIST')) {
    if (!$elfText.Contains($requiredText)) { throw "Wrong smoke79 ELF: missing $requiredText" }
}
if ($elf[0] -ne 0x7f -or $elf[4] -ne 1 -or $elf[5] -ne 1 -or
    [BitConverter]::ToUInt16($elf,18) -ne 243) { throw 'Wrong ELF format' }
$load=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && riscv64-unknown-elf-readelf -l build/program79-app/app.elf")-join "`n"
if ($LASTEXITCODE -ne 0 -or
    $load -notmatch 'LOAD\s+0x[0-9a-f]+\s+0x10400000\s+0x10400000' -or
    $load -match 'INTERP|DYNAMIC') { throw 'Unexpected app load map' }

$ui=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && build/program79-app/pc-ui")
if ($LASTEXITCODE -ne 0) { throw 'UI fixture failed' }
$ui=$ui|ForEach-Object{[regex]::Replace([string]$_,'\x1b\[[0-9;]*[A-Za-z]','')}
$maxWidth=($ui|ForEach-Object{$_.Length}|Measure-Object -Maximum).Maximum
if ($maxWidth -gt 40 -or
    ($ui|Where-Object{$_ -match '^ GBA LAUNCH\+ROM DIAG smoke\.79b [1-5]/5$'}).Count -ne 5) {
    throw "UI contract failed width=$maxWidth"
}

$sd=Join-Path $bundle 'SD_ROOT'
New-Item -ItemType Directory -Path $bundle,$sd | Out-Null
$basePrefix=[IO.Path]::GetFullPath($base).TrimEnd('\')+'\'
foreach ($file in Get-ChildItem -LiteralPath $base -Recurse -File) {
    $rel=$file.FullName.Substring($basePrefix.Length).Replace('\','/')
    if ($rel -in @('Assets/chisburner/common/chisburner78.elf',
                   'Assets/chisburner/common/chisburner78.ini')) { continue }
    $dest=Join-Path $sd $rel
    New-Item -ItemType Directory -Force (Split-Path $dest -Parent) | Out-Null
    Copy-Item -LiteralPath $file.FullName -Destination $dest
}

$common=Join-Path $sd 'Assets/chisburner/common'
$coreRoot=Join-Path $sd 'Cores/ChisBurner.ChisBurner'
if ((Sha (Join-Path $common 'os.bin')) -ne $expectedOsHash -or
    (Sha (Join-Path $coreRoot 'chis.rbf_r')) -ne $expectedRbfHash) {
    throw 'Diagnostic OS/RBF drifted'
}
$osCrc=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes((Join-Path $common 'os.bin')))
Copy-Item -LiteralPath $app -Destination (Join-Path $common 'chisburner79.elf')
[IO.File]::WriteAllText((Join-Path $common 'chisburner79.ini'),
    "[os]`nELF=chisburner79.elf`nARGS=`n",$utf8)
Copy-Item -LiteralPath $readme -Destination (Join-Path $bundle 'README_TEST79_CN.txt')

$corePath=Join-Path $coreRoot 'core.json'
$core=Get-Content -Raw -LiteralPath $corePath | ConvertFrom-Json
$core.core.metadata.version='0.1.0-smoke.79b-playcart-runtime-rom-readonly'
$core.core.metadata.date_release='2026-09-06'
$core.core.metadata.description='Read-only Play Cartridge launch plus runtime ROM slot and target diagnostic'
if ([int64]$core.core.framework.hardware.cartridge_adapter -ne 16777216) {
    throw 'Play Cartridge option is not enabled'
}
[IO.File]::WriteAllText($corePath,($core|ConvertTo-Json -Depth 25),$utf8)

$dataPath=Join-Path $coreRoot 'data.json'
$data=Get-Content -Raw -LiteralPath $dataPath | ConvertFrom-Json
foreach ($slot in $data.data.data_slots) {
    switch ([int]$slot.id) {
        2 { $slot.filename='chisburner79.ini' }
        3 { $slot.filename='chisburner79.elf' }
        4 {
            $slot.name='Program ROM (load after launch)'
            $slot.required=$false
            $slot.parameters=3
            $slot.extensions=@('gba')
            $slot.deferload=$true
            $slot|Add-Member -NotePropertyName size_exact -NotePropertyValue 8705280 -Force
            foreach ($property in @('filename','nonvolatile','address','size_maximum','secondary')) {
                $slot.PSObject.Properties.Remove($property)
            }
        }
    }
}
$slot4=@($data.data.data_slots|Where-Object{[int]$_.id -eq 4})
if ($slot4.Count -ne 1 -or $slot4[0].required -or !$slot4[0].deferload -or
    [int]$slot4[0].parameters -ne 3 -or [int64]$slot4[0].size_exact -ne 8705280 -or
    $slot4[0].PSObject.Properties.Name -contains 'filename') {
    throw 'Runtime ROM slot policy is wrong'
}
[IO.File]::WriteAllText($dataPath,($data|ConvertTo-Json -Depth 25),$utf8)
[IO.File]::WriteAllText((Join-Path $coreRoot 'info.txt'),
    "smoke.79b: start with Play Cartridge, then load exact 8705280-byte ROM through Core Settings slot 4; read-only target diagnostics; no arm, erase, program, or output write.`n",$utf8)

foreach ($json in Get-ChildItem -LiteralPath $sd -Filter '*.json' -Recurse -File) {
    $null=Get-Content -Raw -LiteralPath $json.FullName | ConvertFrom-Json
}
if (Get-ChildItem -LiteralPath $bundle -Recurse -File|Where-Object Extension -ieq '.gba') {
    throw 'ROM leaked into package'
}
[IO.File]::WriteAllText((Join-Path $bundle 'BUILD_REPORT.txt'),
    "smoke.79b package PASS`nmode=read-only combined launch, runtime ROM slot, target identify`nlaunch=Play Cartridge required`nslot4=optional user-reloadable core-specific deferload exact 8705280`nROM bundled=NO`napplication never calls=arm/erase/program/output_write`nOS CRC32=$('{0:x8}' -f $osCrc.ComputedCrc)`nOS SHA256=$expectedOsHash`nRBF_R SHA256=$expectedRbfHash`napp host ASAN/UBSAN=PASS`napp RV32 build=PASS`n",$utf8)
[IO.File]::WriteAllLines((Join-Path $bundle 'UI_FIXTURES_SYNTHETIC.txt'),[string[]]$ui,$utf8)
$manifest=@();$prefix=[IO.Path]::GetFullPath($bundle).TrimEnd('\')+'\'
foreach ($file in Get-ChildItem -LiteralPath $bundle -Recurse -File|Sort-Object FullName) {
    $rel=$file.FullName.Substring($prefix.Length).Replace('\','/')
    $manifest += "$(Sha $file.FullName)  $rel"
}
[IO.File]::WriteAllLines((Join-Path $bundle 'SHA256SUMS.txt'),[string[]]$manifest,$utf8)
Compress-Archive -Path (Join-Path $bundle '*') -DestinationPath $zip -CompressionLevel Optimal
$null=Assert-ChisBurnerZipOsIntegrity -ZipPath $zip
"Package: $zip"
"SHA256: $(Sha $zip)"
"Read-only combined launch/runtime-ROM/target diagnostic; no destructive application calls"
