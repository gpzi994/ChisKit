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
$baseBundle=Join-Path $root 'build/smoke/ChisBurner-smoke.73-s29gl256s-ace-programmer-fix'
$base=Join-Path $baseBundle 'SD_ROOT'
$bundle=Join-Path $root 'build/smoke/ChisBurner-smoke.74-programmer-caps-readonly-diag-v2'
$zip="$bundle.zip"
$app=Join-Path $root 'build/program74-diag-app/app.elf'
$readme=Join-Path $root 'app/gba_program74_diag/README_TEST74_CN.txt'
$utf8=[Text.UTF8Encoding]::new($false)
$baseZipHash='6c79fb5bb53d33f33646d0186ff8d85b8cefdefe2e803f4f128ba62f7f7d8901'
$expectedOsHash='cd1e40641a1d1d211a4681adb83c2d4bde392102b8e5453366d70aa40a0876a3'
$expectedRbfHash='aca0beaa8c9351bb65117c4b1e74a959129b74e15898f382ae2154385c8a242b'

if ((Test-Path -LiteralPath $bundle) -or (Test-Path -LiteralPath $zip)) {
    throw 'Preserve existing smoke74 diagnostic package'
}
foreach ($path in @($base,"$baseBundle.zip",$readme)) {
    if (!(Test-Path -LiteralPath $path)) { throw "Missing package input $path" }
}
if ((Sha "$baseBundle.zip") -ne $baseZipHash) { throw 'Wrong smoke73 base package' }

$diagSource=(Get-Content -Raw -LiteralPath (Join-Path $root 'app/gba_program74_diag/main.c'))+
    (Get-Content -Raw -LiteralPath (Join-Path $root 'app/gba_program67_diag/main.c'))
foreach ($forbiddenCall in @('of_cart_flash_arm','of_cart_flash_erase_sector',
                              'of_cart_flash_program','of_output_write')) {
    if ($diagSource.Contains($forbiddenCall)) {
        throw "Read-only diagnostic contains forbidden call: $forbiddenCall"
    }
}

Wsl "cd '$wroot/app/gba_program74_diag' && make -B host-test all OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0"
Wsl "cd '$wroot' && cc -DOF_PC -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined -fno-omit-frame-pointer -I.integration/openfpgaos/src/firmware/api app/gba_program74_diag/main.c -o build/program74-diag-app/pc-ui-san && build/program74-diag-app/pc-ui-san >/dev/null"

$elf=[IO.File]::ReadAllBytes($app)
$elfText=[Text.Encoding]::ASCII.GetString($elf)
if ($elf[0] -ne 0x7f -or $elf[4] -ne 1 -or $elf[5] -ne 1 -or
    [BitConverter]::ToUInt16($elf,18) -ne 243 -or
    !$elfText.Contains('smoke.74') -or
    !$elfText.Contains('PROGRAMMER CAPS; READ ONLY DIAG') -or
    !$elfText.Contains('NO ARM/ERASE/PROGRAM CALLS EXIST')) {
    throw 'Wrong smoke74 diagnostic application ELF'
}
$load=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && riscv64-unknown-elf-readelf -l build/program74-diag-app/app.elf")-join "`n"
if ($LASTEXITCODE -ne 0 -or
    $load -notmatch 'LOAD\s+0x[0-9a-f]+\s+0x10400000\s+0x10400000' -or
    $load -match 'INTERP|DYNAMIC') { throw 'Unexpected diagnostic application load map' }

$ui=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && build/program74-diag-app/pc-ui")
if ($LASTEXITCODE -ne 0) { throw 'UI fixture failed' }
$ui=$ui|ForEach-Object{[regex]::Replace([string]$_,'\x1b\[[0-9;]*[A-Za-z]','')}
$pages=0;$rows=0;$maxRows=0;$maxWidth=0
foreach ($line in $ui) {
    if ($line -match '^ GBA TARGET DIAG smoke\.74 [1-5]/5$') {
        $maxRows=[Math]::Max($maxRows,$rows);$rows=0;++$pages
    }
    ++$rows;$maxWidth=[Math]::Max($maxWidth,$line.Length)
    if ($line.Length -gt 40) { throw "UI line too wide: $line" }
}
$maxRows=[Math]::Max($maxRows,$rows)
if ($pages -ne 5 -or $maxRows -gt 18) {
    throw "UI shape failed pages=$pages rows=$maxRows width=$maxWidth"
}

$sd=Join-Path $bundle 'SD_ROOT'
New-Item -ItemType Directory -Path $bundle,$sd | Out-Null
$basePrefix=[IO.Path]::GetFullPath($base).TrimEnd('\')+'\'
foreach ($file in Get-ChildItem -LiteralPath $base -Recurse -File) {
    $rel=$file.FullName.Substring($basePrefix.Length).Replace('\','/')
    if ($rel -in @('Assets/chisburner/common/chisburner73.elf',
                   'Assets/chisburner/common/chisburner73.ini')) { continue }
    $dest=Join-Path $sd $rel
    New-Item -ItemType Directory -Force (Split-Path $dest -Parent) | Out-Null
    Copy-Item -LiteralPath $file.FullName -Destination $dest
}

$common=Join-Path $sd 'Assets/chisburner/common'
$coreRoot=Join-Path $sd 'Cores/ChisBurner.ChisBurner'
if ((Sha (Join-Path $common 'os.bin')) -ne $expectedOsHash) {
    throw 'smoke74 must reuse the exact programmer OS from smoke73'
}
if ((Sha (Join-Path $coreRoot 'chis.rbf_r')) -ne $expectedRbfHash) {
    throw 'smoke74 must reuse the exact programmer RBF from smoke73'
}
$osCrc=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes((Join-Path $common 'os.bin')))
Copy-Item -LiteralPath $app -Destination (Join-Path $common 'chisburner74.elf')
[IO.File]::WriteAllText((Join-Path $common 'chisburner74.ini'),
    "[os]`nELF=chisburner74.elf`nARGS=`n",$utf8)
Copy-Item -LiteralPath $readme -Destination (Join-Path $bundle 'README_TEST74_CN.txt')

$corePath=Join-Path $coreRoot 'core.json'
$core=Get-Content -Raw -LiteralPath $corePath | ConvertFrom-Json
$core.core.metadata.version='0.1.0-smoke.74-programmer-caps-readonly-diag'
$core.core.metadata.date_release='2026-09-06'
$core.core.metadata.description='Read-only strict-field diagnostics in exact smoke.73 programmer OS/RBF environment'
[IO.File]::WriteAllText($corePath,($core|ConvertTo-Json -Depth 25),$utf8)

$dataPath=Join-Path $coreRoot 'data.json'
$data=Get-Content -Raw -LiteralPath $dataPath | ConvertFrom-Json
foreach ($slot in $data.data.data_slots) {
    switch ([int]$slot.id) {
        2 { $slot.filename='chisburner74.ini' }
        3 { $slot.filename='chisburner74.elf' }
        4 {
            $slot.name='Unused in smoke.74 read-only diagnostic'
            $slot.required=$false
            $slot.parameters=0
            $slot.extensions=@('bin')
            $slot.deferload=$true
            foreach ($property in @('filename','nonvolatile','address','size_exact',
                                     'size_maximum','secondary')) {
                $slot.PSObject.Properties.Remove($property)
            }
        }
    }
}
[IO.File]::WriteAllText($dataPath,($data|ConvertTo-Json -Depth 25),$utf8)
[IO.File]::WriteAllText((Join-Path $coreRoot 'info.txt'),
    "smoke.74: read-only strict-field target diagnostics in byte-identical smoke.73 programmer OS/RBF; no arm, erase, program, output write, or ROM required.`n",$utf8)

foreach ($json in Get-ChildItem -LiteralPath $sd -Filter '*.json' -Recurse -File) {
    $null=Get-Content -Raw -LiteralPath $json.FullName | ConvertFrom-Json
}
if (Get-ChildItem -LiteralPath $bundle -Recurse -File |
    Where-Object {$_.Extension -ieq '.gba'}) {
    throw 'Diagnostic package must not contain a ROM image'
}

[IO.File]::WriteAllLines((Join-Path $bundle 'UI_FIXTURES_SYNTHETIC.txt'),[string[]]$ui,$utf8)
[IO.File]::WriteAllText((Join-Path $bundle 'BUILD_REPORT.txt'),
    "smoke.74 package PASS`npurpose=locate exact strict field causing smoke.73 RC -4`nbase smoke73 ZIP SHA256=$baseZipHash`nOS SHA256=$expectedOsHash`nOS CRC32=$('{0:x8}' -f $osCrc.ComputedCrc)`nRBF_R SHA256=$expectedRbfHash`napplication calls=get_caps/query/ping/flash_get_caps/safe_idle/identify_read`napplication never calls=arm/erase/program/output_write`nROM required=NO`napp host ASAN/UBSAN=PASS`napp RV32 build=PASS`nui pages=$pages max_columns=$maxWidth max_rows=$maxRows`n",$utf8)

$manifest=@()
$prefix=[IO.Path]::GetFullPath($bundle).TrimEnd('\')+'\'
foreach ($file in Get-ChildItem -LiteralPath $bundle -Recurse -File | Sort-Object FullName) {
    $rel=$file.FullName.Substring($prefix.Length).Replace('\','/')
    $manifest += "$(Sha $file.FullName)  $rel"
}
[IO.File]::WriteAllLines((Join-Path $bundle 'SHA256SUMS.txt'),[string[]]$manifest,$utf8)
Compress-Archive -Path (Join-Path $bundle '*') -DestinationPath $zip -CompressionLevel Optimal
$null=Assert-ChisBurnerZipOsIntegrity -ZipPath $zip
"Package: $zip"
"SHA256: $(Sha $zip)"
"No ROM; exact smoke73 programmer OS/RBF; no destructive application calls"
