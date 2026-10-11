[CmdletBinding()]
param(
    [string]$Distro='Ubuntu-24.04',
    [string]$OpenFpgaSdk='/home/lin13/work/ONS_FPGA/openfpgaSDK',
    [string]$RomPath='C:/Users/lin13/Desktop/game/逆转裁判.gba'
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
$baseBundle=Join-Path $root 'build/smoke/ChisBurner-smoke.79b-playcart-runtime-rom-readonly'
$base=Join-Path $baseBundle 'SD_ROOT'
$bundle=Join-Path $root 'build/smoke/ChisBurner-smoke.80-s29gl256s-ace-runtime-programmer'
$zip="$bundle.zip"
$app=Join-Path $root 'build/program80-app/app.elf'
$readme=Join-Path $root 'app/gba_program80_ace/README_TEST80_CN.txt'
$rom=[IO.Path]::GetFullPath($RomPath)
$wrom='/mnt/'+$rom.Substring(0,1).ToLowerInvariant()+$rom.Substring(2).Replace('\','/')
$utf8=[Text.UTF8Encoding]::new($false)
$expectedRomBytes=8705280L
$expectedRomMd5='07e851b1dc49009ebd00982514ebfe92'
$expectedRomSha='0d0945fff2edd4d4ae627ed003ebb0050937ee689d606ccff89c4ff2c2638571'
$baseZipHash='bc7dd93b05d05a14c85964a7a06c4845f1ae781db701868f618db517cdfed9d8'
$expectedOsHash='cd1e40641a1d1d211a4681adb83c2d4bde392102b8e5453366d70aa40a0876a3'
$expectedRbfHash='aca0beaa8c9351bb65117c4b1e74a959129b74e15898f382ae2154385c8a242b'

if ((Test-Path -LiteralPath $bundle) -or (Test-Path -LiteralPath $zip)) {
    throw 'Preserve existing smoke80 package'
}
foreach ($path in @($base,"$baseBundle.zip",$readme,$rom)) {
    if (!(Test-Path -LiteralPath $path)) { throw "Missing package input $path" }
}
if ((Sha "$baseBundle.zip") -ne $baseZipHash) { throw 'Wrong smoke79b base package' }
if ((Get-Item -LiteralPath $rom).Length -ne $expectedRomBytes -or
    (Get-FileHash -LiteralPath $rom -Algorithm MD5).Hash.ToLowerInvariant() -ne $expectedRomMd5 -or
    (Sha $rom) -ne $expectedRomSha) { throw 'Wrong test ROM' }

Wsl "cd '$wroot/app/gba_program80_ace' && make -B host-test all OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0 ROM_PATH='$wrom'"
$elf=[IO.File]::ReadAllBytes($app)
$elfText=[Text.Encoding]::ASCII.GetString($elf)
foreach ($requiredText in @('smoke.80','WAITING FOR PROGRAM ROM',
                             'LOAD PROGRAM ROM (SLOT 4)',
                             'FULL ERASE %u SECTORS',
                             'A: ERASE+PROGRAM  B: CANCEL/RELOCK')) {
    if (!$elfText.Contains($requiredText)) {
        throw "Wrong smoke80 ELF: missing $requiredText"
    }
}
if ($elf[0] -ne 0x7f -or $elf[4] -ne 1 -or $elf[5] -ne 1 -or
    [BitConverter]::ToUInt16($elf,18) -ne 243) { throw 'Wrong ELF format' }
$load=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && riscv64-unknown-elf-readelf -l build/program80-app/app.elf")-join "`n"
if ($LASTEXITCODE -ne 0 -or
    $load -notmatch 'LOAD\s+0x[0-9a-f]+\s+0x10400000\s+0x10400000' -or
    $load -match 'INTERP|DYNAMIC') { throw 'Unexpected app load map' }

$ui=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && build/program80-app/pc-ui")
if ($LASTEXITCODE -ne 0) { throw 'UI fixture failed' }
$ui=$ui|ForEach-Object{[regex]::Replace([string]$_,'\x1b\[[0-9;]*[A-Za-z]','')}
$maxWidth=($ui|ForEach-Object{$_.Length}|Measure-Object -Maximum).Maximum
if ($maxWidth -gt 40 -or ($ui|Where-Object{$_ -eq ' GBA PROGRAM smoke.80'}).Count -ne 11) {
    throw "UI contract failed width=$maxWidth"
}

$sd=Join-Path $bundle 'SD_ROOT'
New-Item -ItemType Directory -Path $bundle,$sd | Out-Null
$basePrefix=[IO.Path]::GetFullPath($base).TrimEnd('\')+'\'
foreach ($file in Get-ChildItem -LiteralPath $base -Recurse -File) {
    $rel=$file.FullName.Substring($basePrefix.Length).Replace('\','/')
    if ($rel -in @('Assets/chisburner/common/chisburner79.elf',
                   'Assets/chisburner/common/chisburner79.ini')) { continue }
    $dest=Join-Path $sd $rel
    New-Item -ItemType Directory -Force (Split-Path $dest -Parent) | Out-Null
    Copy-Item -LiteralPath $file.FullName -Destination $dest
}
$common=Join-Path $sd 'Assets/chisburner/common'
$coreRoot=Join-Path $sd 'Cores/ChisBurner.ChisBurner'
if ((Sha (Join-Path $common 'os.bin')) -ne $expectedOsHash -or
    (Sha (Join-Path $coreRoot 'chis.rbf_r')) -ne $expectedRbfHash) {
    throw 'Programmer OS/RBF drifted'
}
$osCrc=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes((Join-Path $common 'os.bin')))
Copy-Item -LiteralPath $app -Destination (Join-Path $common 'chisburner80.elf')
[IO.File]::WriteAllText((Join-Path $common 'chisburner80.ini'),
    "[os]`nELF=chisburner80.elf`nARGS=`n",$utf8)
Copy-Item -LiteralPath $readme -Destination (Join-Path $bundle 'README_TEST80_CN.txt')

$corePath=Join-Path $coreRoot 'core.json'
$core=Get-Content -Raw -LiteralPath $corePath | ConvertFrom-Json
$core.core.metadata.version='0.1.0-smoke.80-s29gl256s-ace-runtime-programmer'
$core.core.metadata.date_release='2026-09-06'
$core.core.metadata.description='S29GL256S full-chip sector erase and 8.7 MB runtime-loaded ROM programmer'
if ([int64]$core.core.framework.hardware.cartridge_adapter -ne 16777216) {
    throw 'Play Cartridge option is not enabled'
}
[IO.File]::WriteAllText($corePath,($core|ConvertTo-Json -Depth 25),$utf8)
$dataPath=Join-Path $coreRoot 'data.json'
$data=Get-Content -Raw -LiteralPath $dataPath | ConvertFrom-Json
foreach ($slot in $data.data.data_slots) {
    if ([int]$slot.id -eq 2) { $slot.filename='chisburner80.ini' }
    if ([int]$slot.id -eq 3) { $slot.filename='chisburner80.elf' }
}
$slot4=@($data.data.data_slots|Where-Object{[int]$_.id -eq 4})
if ($slot4.Count -ne 1 -or $slot4[0].required -or !$slot4[0].deferload -or
    [int]$slot4[0].parameters -ne 3 -or [int64]$slot4[0].size_exact -ne 8705280 -or
    $slot4[0].PSObject.Properties.Name -contains 'filename') {
    throw 'Runtime ROM slot policy drifted from smoke79b'
}
[IO.File]::WriteAllText($dataPath,($data|ConvertTo-Json -Depth 25),$utf8)
[IO.File]::WriteAllText((Join-Path $coreRoot 'info.txt'),
    "smoke.80: destructive S29GL256S programmer; Play Cartridge launch plus runtime-loaded exact 8705280-byte ROM; explicit two-stage physical confirmation; full 32 MiB erase as 256 x 128 KiB sectors; 32-byte program blocks; final MD5 verification.`n",$utf8)

foreach ($json in Get-ChildItem -LiteralPath $sd -Filter '*.json' -Recurse -File) {
    $null=Get-Content -Raw -LiteralPath $json.FullName | ConvertFrom-Json
}
if (Get-ChildItem -LiteralPath $bundle -Recurse -File|Where-Object Extension -ieq '.gba') {
    throw 'ROM leaked into package'
}
[IO.File]::WriteAllText((Join-Path $bundle 'BUILD_REPORT.txt'),
    "smoke.80 package PASS`nmode=destructive S29GL256S programmer after explicit confirmation`nlaunch=Play Cartridge required`nslot4=optional user-reloadable core-specific deferload exact 8705280`nROM bytes=$expectedRomBytes`nROM MD5=$expectedRomMd5`nROM SHA256=$expectedRomSha`nfull erase=33554432 bytes, 256 x 131072-byte sectors`nprogram=8705280 bytes, 272040 x 32-byte blocks`nverify=8705280-byte cartridge MD5`nROM bundled=NO`nOS CRC32=$('{0:x8}' -f $osCrc.ComputedCrc)`nOS SHA256=$expectedOsHash`nRBF_R SHA256=$expectedRbfHash`napp RV32 and ASAN/UBSAN program model=PASS`n",$utf8)
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
"Destructive programmer: full target erase starts only after all gates and explicit physical confirmation"
