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
$baseBundle=Join-Path $root 'build/smoke/ChisBurner-smoke.73-s29gl256s-ace-programmer-fix'
$base=Join-Path $baseBundle 'SD_ROOT'
$bundle=Join-Path $root 'build/smoke/ChisBurner-smoke.75-s29gl256s-ace-programmer-preflight'
$zip="$bundle.zip"
$app=Join-Path $root 'build/program75-app/app.elf'
$readme=Join-Path $root 'app/gba_program75_ace/README_TEST75_CN.txt'
$rom=[IO.Path]::GetFullPath($RomPath)
$wrom='/mnt/'+$rom.Substring(0,1).ToLowerInvariant()+$rom.Substring(2).Replace('\','/')
$utf8=[Text.UTF8Encoding]::new($false)
$expectedRomBytes=8705280L
$expectedRomMd5='07e851b1dc49009ebd00982514ebfe92'
$expectedRomSha='0d0945fff2edd4d4ae627ed003ebb0050937ee689d606ccff89c4ff2c2638571'
$baseZipHash='6c79fb5bb53d33f33646d0186ff8d85b8cefdefe2e803f4f128ba62f7f7d8901'
$expectedOsHash='cd1e40641a1d1d211a4681adb83c2d4bde392102b8e5453366d70aa40a0876a3'
$expectedRbfHash='aca0beaa8c9351bb65117c4b1e74a959129b74e15898f382ae2154385c8a242b'

if ((Test-Path -LiteralPath $bundle) -or (Test-Path -LiteralPath $zip)) {
    throw 'Preserve existing smoke75 package'
}
foreach ($path in @($base,"$baseBundle.zip",$readme,$rom)) {
    if (!(Test-Path -LiteralPath $path)) { throw "Missing package input $path" }
}
if ((Sha "$baseBundle.zip") -ne $baseZipHash) { throw 'Wrong smoke73 base package' }
if ((Get-Item -LiteralPath $rom).Length -ne $expectedRomBytes -or
    (Get-FileHash -LiteralPath $rom -Algorithm MD5).Hash.ToLowerInvariant() -ne $expectedRomMd5 -or
    (Sha $rom) -ne $expectedRomSha) {
    throw 'The selected Ace Attorney ROM does not match the tested source'
}

Wsl "cd '$wroot/app/gba_program75_ace' && make -B host-test all OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0 ROM_PATH='$wrom'"

$elf=[IO.File]::ReadAllBytes($app)
$elfText=[Text.Encoding]::ASCII.GetString($elf)
foreach ($requiredText in @('smoke.75','TARGET S29GL256S 32 MiB',
                             'PROTO %d PING %d SAFE %d',
                             'FULL ERASE %u SECTORS',
                             'A: ERASE+PROGRAM  B: CANCEL/RELOCK')) {
    if (!$elfText.Contains($requiredText)) {
        throw "Wrong smoke75 application ELF: missing $requiredText"
    }
}
if ($elf[0] -ne 0x7f -or $elf[4] -ne 1 -or $elf[5] -ne 1 -or
    [BitConverter]::ToUInt16($elf,18) -ne 243) {
    throw 'Wrong smoke75 application ELF format'
}
$load=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && riscv64-unknown-elf-readelf -l build/program75-app/app.elf")-join "`n"
if ($LASTEXITCODE -ne 0 -or
    $load -notmatch 'LOAD\s+0x[0-9a-f]+\s+0x10400000\s+0x10400000' -or
    $load -match 'INTERP|DYNAMIC') { throw 'Unexpected smoke75 application load map' }

$ui=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && build/program75-app/pc-ui")
if ($LASTEXITCODE -ne 0) { throw 'UI fixture failed' }
$ui=$ui|ForEach-Object{[regex]::Replace([string]$_,'\x1b\[[0-9;]*[A-Za-z]','')}
$pages=0;$rows=0;$maxRows=0;$maxWidth=0
foreach ($line in $ui) {
    if ($line -eq ' GBA PROGRAM smoke.75') {
        $maxRows=[Math]::Max($maxRows,$rows);$rows=0;++$pages
    }
    ++$rows;$maxWidth=[Math]::Max($maxWidth,$line.Length)
    if ($line.Length -gt 40) { throw "UI line too wide: $line" }
}
$maxRows=[Math]::Max($maxRows,$rows)
foreach ($required in @('ERASE 256/256','PROGRAM 272040/272040',
                         'VERIFY 8705280/8705280')) {
    if (!($ui -contains $required)) { throw "Missing smoke75 UI contract: $required" }
}
if ($pages -ne 11 -or $maxRows -gt 18) {
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
    throw 'smoke75 must reuse the exact programmer OS proven by smoke74'
}
if ((Sha (Join-Path $coreRoot 'chis.rbf_r')) -ne $expectedRbfHash) {
    throw 'smoke75 must reuse the exact programmer RBF proven by smoke74'
}
$osCrc=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes((Join-Path $common 'os.bin')))
Copy-Item -LiteralPath $app -Destination (Join-Path $common 'chisburner75.elf')
[IO.File]::WriteAllText((Join-Path $common 'chisburner75.ini'),
    "[os]`nELF=chisburner75.elf`nARGS=`n",$utf8)
Copy-Item -LiteralPath $readme -Destination (Join-Path $bundle 'README_TEST75_CN.txt')

$corePath=Join-Path $coreRoot 'core.json'
$core=Get-Content -Raw -LiteralPath $corePath | ConvertFrom-Json
$core.core.metadata.version='0.1.0-smoke.75-s29gl256s-ace-programmer-preflight'
$core.core.metadata.date_release='2026-09-06'
$core.core.metadata.description='S29GL256S full-chip erase/program test with smoke.74-proven read-only protocol preflight'
[IO.File]::WriteAllText($corePath,($core|ConvertTo-Json -Depth 25),$utf8)

$dataPath=Join-Path $coreRoot 'data.json'
$data=Get-Content -Raw -LiteralPath $dataPath | ConvertFrom-Json
foreach ($slot in $data.data.data_slots) {
    switch ([int]$slot.id) {
        2 { $slot.filename='chisburner75.ini' }
        3 { $slot.filename='chisburner75.elf' }
        4 {
            $slot.name='8705280 B GBA ROM TO PROGRAM'
            $slot.required=$true
            $slot.parameters=2
            $slot.extensions=@('gba')
            $slot.deferload=$true
            if ($slot.PSObject.Properties['size_exact']) {
                $slot.size_exact=$expectedRomBytes
            } else {
                $slot | Add-Member -NotePropertyName size_exact -NotePropertyValue $expectedRomBytes
            }
            foreach ($property in @('filename','nonvolatile','address',
                                     'size_maximum','secondary')) {
                $slot.PSObject.Properties.Remove($property)
            }
        }
    }
}
[IO.File]::WriteAllText($dataPath,($data|ConvertTo-Json -Depth 25),$utf8)
[IO.File]::WriteAllText((Join-Path $coreRoot 'info.txt'),
    "smoke.75: exact smoke.74-proven programmer OS/RBF; strict read-only protocol preflight; full 32 MiB erase; exact 8705280-byte ROM program in 32-byte operations; final cartridge MD5 verification.`n",$utf8)

foreach ($json in Get-ChildItem -LiteralPath $sd -Filter '*.json' -Recurse -File) {
    $null=Get-Content -Raw -LiteralPath $json.FullName | ConvertFrom-Json
}
if (Get-ChildItem -LiteralPath $bundle -Recurse -File |
    Where-Object {$_.Extension -ieq '.gba'}) {
    throw 'ROM must not be included in the package'
}

[IO.File]::WriteAllText((Join-Path $bundle 'BUILD_REPORT.txt'),
    "smoke.75 package PASS`nsmoke74 hardware result=cart strict PASS; flash strict PASS`npreflight=cart caps; protocol query; ping; flash ID/CFI; safe idle`nROM bytes=$expectedRomBytes`nROM MD5=$expectedRomMd5`nROM SHA256=$expectedRomSha`nfull erase=256 x 128 KiB = 32 MiB`nprogram operation=32 B`nOS CRC32=$('{0:x8}' -f $osCrc.ComputedCrc)`nOS SHA256=$expectedOsHash`nRBF_R SHA256=$expectedRbfHash`nprogram model ASAN/UBSAN=PASS`napp RV32 build=PASS`nui pages=$pages max_columns=$maxWidth max_rows=$maxRows`n",$utf8)
[IO.File]::WriteAllLines((Join-Path $bundle 'UI_FIXTURES_SYNTHETIC.txt'),[string[]]$ui,$utf8)

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
"Full 32 MiB erase after explicit confirmation; ROM not included"
