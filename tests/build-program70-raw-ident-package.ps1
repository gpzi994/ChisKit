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
$baseBundle=Join-Path $root 'build/smoke/ChisBurner-smoke.66-s29gl256-programmer-preview'
$base=Join-Path $baseBundle 'SD_ROOT'
$bundle=Join-Path $root 'build/smoke/ChisBurner-smoke.70-raw-ident-space-fix'
$zip="$bundle.zip"
$app=Join-Path $root 'build/program70-raw-ident-app/app.elf'
$utf8=[Text.UTF8Encoding]::new($false)
$baseZipHash='e3d2213463311a9e6cc140a5329dee7c165a3ded3c52c076486fc6ee432506d2'
$osHash='8d2f03f6a32584c6bd6c2a6fc87242928870aa248d83cb7afc2f7ae85b3c5662'
$rbfHash='fe73198c5e9164d9760ea62fd02ba671022b1083b028f99c62ca1cd949eca842'

if ((Test-Path -LiteralPath $bundle) -or (Test-Path -LiteralPath $zip)) {
    throw 'Preserve existing smoke70 diagnostic package'
}
if (!(Test-Path -LiteralPath $base) -or (Sha "$baseBundle.zip") -ne $baseZipHash) {
    throw 'Wrong or missing smoke66 base package'
}
if ((Sha (Join-Path $base 'Assets/chisburner/common/os.bin')) -ne $osHash -or
    (Sha (Join-Path $base 'Cores/ChisBurner.ChisBurner/chis.rbf_r')) -ne $rbfHash) {
    throw 'Smoke66 known-good OS/RBF input changed'
}

$wrapper=Get-Content -Raw -LiteralPath (Join-Path $root 'app/gba_program70_raw_ident/main.c')
$implementation=Get-Content -Raw -LiteralPath (Join-Path $root 'app/gba_program69_raw_ident/main.c')
if ($wrapper -notmatch '#define D69_SPACE_ADDRESS 0x4000015c') {
    throw 'smoke70 does not use physical CART_SPACE address 0x4000015C'
}
foreach ($forbidden in @('of_cart_flash_arm','of_cart_flash_erase_sector','of_cart_flash_program',
                          'D69_REG_TX_DATA','D69_REG_ARM','CMD_ERASE','CMD_PROGRAM')) {
    if (($wrapper+$implementation).Contains($forbidden)) {
        throw "Forbidden write primitive in diagnostic: $forbidden"
    }
}
Wsl "cd '$wroot' && bash tests/run-program65-periph-rtl.sh"
Wsl "cd '$wroot/app/gba_program70_raw_ident' && make host-test all OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0"
Wsl "cd '$wroot' && cc -DOF_PC -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined -fno-omit-frame-pointer -I.integration/openfpgaos/src/firmware/api app/gba_program70_raw_ident/main.c -o build/program70-raw-ident-app/pc-ui-san && build/program70-raw-ident-app/pc-ui-san >/dev/null"

$elf=[IO.File]::ReadAllBytes($app)
if ($elf[0] -ne 0x7f -or $elf[4] -ne 1 -or $elf[5] -ne 1 -or
    [BitConverter]::ToUInt16($elf,18) -ne 243 -or
    ![Text.Encoding]::ASCII.GetString($elf).Contains('smoke.70')) {
    throw 'Wrong smoke70 application ELF'
}
$load=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && riscv64-unknown-elf-readelf -l build/program70-raw-ident-app/app.elf")-join "`n"
if ($LASTEXITCODE -ne 0 -or
    $load -notmatch 'LOAD\s+0x[0-9a-f]+\s+0x10400000\s+0x10400000' -or
    $load -match 'INTERP|DYNAMIC') { throw 'Unexpected application load map' }

$ui=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && build/program70-raw-ident-app/pc-ui")
if ($LASTEXITCODE -ne 0) { throw 'UI fixture failed' }
$ui=$ui|ForEach-Object{[regex]::Replace([string]$_,'\x1b\[[0-9;]*[A-Za-z]','')}
$pages=0;$rows=0;$maxRows=0;$maxWidth=0
foreach ($line in $ui) {
    if ($line -match '^ GBA RAW IDENT smoke\.70 [1-4]/4$') {
        $maxRows=[Math]::Max($maxRows,$rows);$rows=0;++$pages
    }
    ++$rows;$maxWidth=[Math]::Max($maxWidth,$line.Length)
    if ($line.Length -gt 40) { throw "UI line too wide: $line" }
}
$maxRows=[Math]::Max($maxRows,$rows)
if ($pages -ne 4 -or $maxRows -gt 18) { throw "UI shape failed pages=$pages rows=$maxRows" }

$sd=Join-Path $bundle 'SD_ROOT'
New-Item -ItemType Directory -Path $bundle,$sd | Out-Null
$basePrefix=[IO.Path]::GetFullPath($base).TrimEnd('\')+'\'
foreach ($file in Get-ChildItem -LiteralPath $base -Recurse -File) {
    $rel=$file.FullName.Substring($basePrefix.Length).Replace('\','/')
    if ($rel -in @('Assets/chisburner/common/chisburner66.elf','Assets/chisburner/common/chisburner66.ini')) { continue }
    $dest=Join-Path $sd $rel
    New-Item -ItemType Directory -Force (Split-Path $dest -Parent) | Out-Null
    Copy-Item -LiteralPath $file.FullName -Destination $dest
}

$common=Join-Path $sd 'Assets/chisburner/common'
$coreRoot=Join-Path $sd 'Cores/ChisBurner.ChisBurner'
Copy-Item -LiteralPath $app -Destination (Join-Path $common 'chisburner70.elf')
[IO.File]::WriteAllText((Join-Path $common 'chisburner70.ini'),"[os]`nELF=chisburner70.elf`nARGS=`n",$utf8)
if ((Sha (Join-Path $common 'os.bin')) -ne $osHash -or
    (Sha (Join-Path $coreRoot 'chis.rbf_r')) -ne $rbfHash) {
    throw 'Known-good smoke66 OS/RBF changed while packaging'
}
$osCrc=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes((Join-Path $common 'os.bin')))

$corePath=Join-Path $coreRoot 'core.json'
$core=Get-Content -Raw -LiteralPath $corePath | ConvertFrom-Json
$core.core.metadata.version='0.1.0-smoke.70-raw-ident-space-fix'
$core.core.metadata.date_release='2026-09-05'
$core.core.metadata.description='Raw Identify read with physical CART_SPACE mapping fixed'
[IO.File]::WriteAllText($corePath,($core|ConvertTo-Json -Depth 25),$utf8)
$dataPath=Join-Path $coreRoot 'data.json'
$data=Get-Content -Raw -LiteralPath $dataPath | ConvertFrom-Json
foreach ($slot in $data.data.data_slots) {
    switch ([int]$slot.id) {
        2 { $slot.filename='chisburner70.ini' }
        3 { $slot.filename='chisburner70.elf' }
        4 {
            $slot.name='Unused in Identify read-only diagnostic';$slot.required=$false
            $slot.parameters=0;$slot.extensions=@('bin');$slot.deferload=$true
            foreach ($property in @('filename','nonvolatile','address','size_exact','size_maximum','secondary')) {
                $slot.PSObject.Properties.Remove($property)
            }
        }
    }
}
[IO.File]::WriteAllText($dataPath,($data|ConvertTo-Json -Depth 25),$utf8)
[IO.File]::WriteAllText((Join-Path $coreRoot 'info.txt'),
    "smoke.70 corrected CART_SPACE mapping; raw Identify read only; exact smoke.66 OS/RBF.`n",$utf8)
foreach ($json in Get-ChildItem -LiteralPath $sd -Filter '*.json' -Recurse -File) {
    $null=Get-Content -Raw -LiteralPath $json.FullName | ConvertFrom-Json
}
if (Get-ChildItem -LiteralPath $sd -Recurse -File | Where-Object Extension -eq '.gba') {
    throw 'Diagnostic package must not contain a ROM image'
}

Copy-Item -LiteralPath (Join-Path $root 'app/gba_program70_raw_ident/README_TEST70_CN.txt') -Destination $bundle
[IO.File]::WriteAllLines((Join-Path $bundle 'UI_FIXTURES_SYNTHETIC.txt'),[string[]]$ui,$utf8)
$proof=@"
smoke=70-raw-ident-space-fix
smoke69_finding=read ROM header because logical CART_SPACE physical mapping was wrong
cart_space_physical_address=0x4000015c
peripheral_mapping_test=PASS
application_commands=SAFE_IDLE;READ_IDENTIFY;SAFE_IDLE
application_never_commands=ARM;ERASE;PROGRAM
source_rom_required=NO
os_sha256=$(Sha (Join-Path $common 'os.bin'))
os_internal_crc=$('{0:x8}' -f $osCrc.ComputedCrc)
fpga_rbf_r_sha256=$(Sha (Join-Path $coreRoot 'chis.rbf_r'))
app_sha256=$(Sha (Join-Path $common 'chisburner70.elf'))
app_host_asan_ubsan=PASS
app_rv32_build=PASS
ui_pages=$pages;max_columns=$maxWidth;max_rows=$maxRows
"@
[IO.File]::WriteAllText((Join-Path $bundle 'BUILD_PROVENANCE.txt'),$proof,$utf8)

$sdPrefix=[IO.Path]::GetFullPath($sd).TrimEnd('\')+'\';$hashes=@{}
$sums=Get-ChildItem -LiteralPath $sd -Recurse -File|Sort-Object FullName|ForEach-Object{
    $rel=$_.FullName.Substring($sdPrefix.Length).Replace('\','/');$hash=Sha $_.FullName
    $hashes["SD_ROOT/$rel"]=$hash;"$hash  $rel"
}
[IO.File]::WriteAllLines((Join-Path $bundle 'SHA256SUMS.txt'),[string[]]$sums,$utf8)
Compress-Archive -Path (Join-Path $bundle '*') -DestinationPath $zip -CompressionLevel Optimal
$null=Assert-ChisBurnerZipOsIntegrity -ZipPath $zip
Add-Type -AssemblyName System.IO.Compression.FileSystem
$archive=[IO.Compression.ZipFile]::OpenRead($zip)
try {
    if ($archive.Entries|Where-Object{$_.FullName.Replace('\','/') -match '\.gba$'}) { throw 'ZIP contains a ROM image' }
    foreach ($name in $hashes.Keys) {
        $entry=$archive.GetEntry($name);if (!$entry) { throw "ZIP missing $name" }
        $stream=$entry.Open();$sha=[Security.Cryptography.SHA256]::Create()
        try {
            $actual=[BitConverter]::ToString($sha.ComputeHash($stream)).Replace('-','').ToLowerInvariant()
            if ($actual -ne $hashes[$name]) { throw "ZIP mismatch $name" }
        } finally { $stream.Dispose();$sha.Dispose() }
    }
} finally { $archive.Dispose() }

"PASS: $zip"
"ZIP SHA256: $(Sha $zip)"
"Correct CART_SPACE mapping; exact smoke66 OS/RBF; Identify READ only; $($hashes.Count) SD files verified"
