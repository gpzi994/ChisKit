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
$baseBundle=Join-Path $root 'build/smoke/ChisBurner-smoke.77-lifecycle-stable-preflight'
$base=Join-Path $baseBundle 'SD_ROOT'
$bundle=Join-Path $root 'build/smoke/ChisBurner-smoke.78-b1-reset-readonly-diag'
$zip="$bundle.zip"
$app=Join-Path $root 'build/program78-app/app.elf'
$readme=Join-Path $root 'app/gba_program78_b1diag/README_TEST78_CN.txt'
$utf8=[Text.UTF8Encoding]::new($false)
$expectedOsHash='cd1e40641a1d1d211a4681adb83c2d4bde392102b8e5453366d70aa40a0876a3'
$expectedRbfHash='aca0beaa8c9351bb65117c4b1e74a959129b74e15898f382ae2154385c8a242b'

if ((Test-Path -LiteralPath $bundle) -or (Test-Path -LiteralPath $zip)) {
    throw 'Preserve existing smoke78 package'
}
foreach ($path in @($base,$readme)) {
    if (!(Test-Path -LiteralPath $path)) { throw "Missing package input $path" }
}

Wsl "cd '$wroot/app/gba_program78_b1diag' && make -B host-test all OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0"
$elf=[IO.File]::ReadAllBytes($app)
$elfText=[Text.Encoding]::ASCII.GetString($elf)
foreach ($requiredText in @('smoke.78','COUNT B1/ENTER/EXIT','B1 SEEN THEN INVALIDATED',
                             'READ ONLY; NO FLASH COMMANDS')) {
    if (!$elfText.Contains($requiredText)) {
        throw "Wrong smoke78 ELF: missing $requiredText"
    }
}
$sourceText=Get-Content -Raw -LiteralPath (Join-Path $root 'app/gba_program78_b1diag/main.c')
if (!$sourceText.Contains('OF_SVC->file_size("slot:4")')) {
    throw 'Missing read-only slot-size chronology probe'
}
foreach ($forbiddenText in @('FULL ERASE','PROGRAM COMPLETE','CONFIRM ERASE')) {
    if ($elfText.Contains($forbiddenText)) {
        throw "Unsafe programmer text leaked into smoke78: $forbiddenText"
    }
}
if ($elf[0] -ne 0x7f -or $elf[4] -ne 1 -or $elf[5] -ne 1 -or
    [BitConverter]::ToUInt16($elf,18) -ne 243) { throw 'Wrong ELF format' }
$load=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && riscv64-unknown-elf-readelf -l build/program78-app/app.elf")-join "`n"
if ($LASTEXITCODE -ne 0 -or
    $load -notmatch 'LOAD\s+0x[0-9a-f]+\s+0x10400000\s+0x10400000' -or
    $load -match 'INTERP|DYNAMIC') { throw 'Unexpected app load map' }

$ui=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && build/program78-app/pc-ui")
if ($LASTEXITCODE -ne 0) { throw 'UI fixture failed' }
$ui=$ui|ForEach-Object{[regex]::Replace([string]$_,'\x1b\[[0-9;]*[A-Za-z]','')}
$maxWidth=($ui|ForEach-Object{$_.Length}|Measure-Object -Maximum).Maximum
if ($maxWidth -gt 40 -or ($ui|Where-Object{$_ -match '^ GBA B1/RESET DIAG smoke\.78 [1-5]/5$'}).Count -ne 5) {
    throw "UI contract failed width=$maxWidth"
}

$sd=Join-Path $bundle 'SD_ROOT'
New-Item -ItemType Directory -Path $bundle,$sd | Out-Null
$basePrefix=[IO.Path]::GetFullPath($base).TrimEnd('\')+'\'
foreach ($file in Get-ChildItem -LiteralPath $base -Recurse -File) {
    $rel=$file.FullName.Substring($basePrefix.Length).Replace('\','/')
    if ($rel -in @('Assets/chisburner/common/chisburner77.elf',
                   'Assets/chisburner/common/chisburner77.ini')) { continue }
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
Copy-Item -LiteralPath $app -Destination (Join-Path $common 'chisburner78.elf')
[IO.File]::WriteAllText((Join-Path $common 'chisburner78.ini'),
    "[os]`nELF=chisburner78.elf`nARGS=`n",$utf8)
Copy-Item -LiteralPath $readme -Destination (Join-Path $bundle 'README_TEST78_CN.txt')

$corePath=Join-Path $coreRoot 'core.json'
$core=Get-Content -Raw -LiteralPath $corePath | ConvertFrom-Json
$core.core.metadata.version='0.1.0-smoke.78-b1-reset-readonly-diag'
$core.core.metadata.date_release='2026-09-06'
$core.core.metadata.description='Read-only B1 and Reset chronology diagnostic for required ROM slot startup'
[IO.File]::WriteAllText($corePath,($core|ConvertTo-Json -Depth 25),$utf8)
$dataPath=Join-Path $coreRoot 'data.json'
$data=Get-Content -Raw -LiteralPath $dataPath | ConvertFrom-Json
foreach ($slot in $data.data.data_slots) {
    if ([int]$slot.id -eq 2) { $slot.filename='chisburner78.ini' }
    if ([int]$slot.id -eq 3) { $slot.filename='chisburner78.elf' }
}
[IO.File]::WriteAllText($dataPath,($data|ConvertTo-Json -Depth 25),$utf8)
[IO.File]::WriteAllText((Join-Path $coreRoot 'info.txt'),
    "smoke.78: read-only B1/Reset timeline; required 8705280-byte GBA slot retained; no protocol, ROM, identify, arm, erase, or program command.`n",$utf8)
foreach ($json in Get-ChildItem -LiteralPath $sd -Filter '*.json' -Recurse -File) {
    $null=Get-Content -Raw -LiteralPath $json.FullName | ConvertFrom-Json
}
if (Get-ChildItem -LiteralPath $bundle -Recurse -File|Where-Object Extension -ieq '.gba') {
    throw 'ROM leaked into package'
}
[IO.File]::WriteAllText((Join-Path $bundle 'BUILD_REPORT.txt'),
    "smoke.78 package PASS`nmode=read-only B1/Reset chronology`nsnapshots=entry, 250 ms, post file_size(slot:4), late 1.5 s`nROM slot=required exact 8705280 bytes; ROM not bundled`nOS CRC32=$('{0:x8}' -f $osCrc.ComputedCrc)`nOS SHA256=$expectedOsHash`nRBF_R SHA256=$expectedRbfHash`napp RV32 and PC UI=PASS`n",$utf8)
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
"Read-only chronology diagnostic; no cartridge command path is called"
