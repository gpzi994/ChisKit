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
$bundle=Join-Path $root 'build/smoke/ChisBurner-smoke.67-target-ident-readonly-diag'
$zip="$bundle.zip"
$os=Join-Path $root '.integration/openfpgaos/src/firmware/os/bld/program67-pocket-diag/os.bin'
$app=Join-Path $root 'build/program67-diag-app/app.elf'
$utf8=[Text.UTF8Encoding]::new($false)

if ((Test-Path -LiteralPath $bundle) -or (Test-Path -LiteralPath $zip)) {
    throw 'Preserve existing smoke67 diagnostic package'
}
foreach ($path in @($base,$os)) {
    if (!(Test-Path -LiteralPath $path)) { throw "Missing package input $path" }
}

$diagSource=Get-Content -Raw -LiteralPath (Join-Path $root 'app/gba_program67_diag/main.c')
foreach ($forbiddenCall in @('of_cart_flash_erase_sector',
                              'of_cart_flash_program')) {
    if ($diagSource.Contains($forbiddenCall)) {
        throw "Read-only diagnostic contains forbidden call: $forbiddenCall"
    }
}

Wsl "cd '$wroot' && cc -O2 -Wall -Wextra -Werror -I.integration/openfpgaos/src/firmware/api .integration/openfpgaos/tools/test_cart_api.c -o build/program67-cart-api-test && build/program67-cart-api-test"
Wsl "cd '$wroot/app/gba_program67_diag' && make host-test all OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0"
Wsl "cd '$wroot' && cc -DOF_PC -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined -fno-omit-frame-pointer -I.integration/openfpgaos/src/firmware/api app/gba_program67_diag/main.c -o build/program67-diag-app/pc-ui-san && build/program67-diag-app/pc-ui-san >/dev/null"

$elf=[IO.File]::ReadAllBytes($app)
if ($elf[0] -ne 0x7f -or $elf[4] -ne 1 -or $elf[5] -ne 1 -or
    [BitConverter]::ToUInt16($elf,18) -ne 243 -or
    ![Text.Encoding]::ASCII.GetString($elf).Contains('GBA TARGET DIAG smoke.67')) {
    throw 'Wrong diagnostic application ELF'
}
$load=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && riscv64-unknown-elf-readelf -l build/program67-diag-app/app.elf")-join "`n"
if ($LASTEXITCODE -ne 0 -or
    $load -notmatch 'LOAD\s+0x[0-9a-f]+\s+0x10400000\s+0x10400000' -or
    $load -match 'INTERP|DYNAMIC') {
    throw 'Unexpected diagnostic application load map'
}

$ui=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && build/program67-diag-app/pc-ui")
if ($LASTEXITCODE -ne 0) { throw 'UI fixture failed' }
$ui=$ui|ForEach-Object{[regex]::Replace([string]$_,'\x1b\[[0-9;]*[A-Za-z]','')}
$pages=0;$rows=0;$maxRows=0;$maxWidth=0
foreach ($line in $ui) {
    if ($line -match '^ GBA TARGET DIAG smoke\.67 [1-5]/5$') {
        $maxRows=[Math]::Max($maxRows,$rows);$rows=0;++$pages
    }
    ++$rows
    $maxWidth=[Math]::Max($maxWidth,$line.Length)
    if ($line.Length -gt 40) { throw "UI line too wide: $line" }
}
$maxRows=[Math]::Max($maxRows,$rows)
if ($pages -ne 5 -or $maxRows -gt 18) {
    throw "UI shape failed pages=$pages rows=$maxRows"
}

$osCrc=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes($os))

$sd=Join-Path $bundle 'SD_ROOT'
New-Item -ItemType Directory -Path $bundle,$sd | Out-Null
$basePrefix=[IO.Path]::GetFullPath($base).TrimEnd('\')+'\'
foreach ($file in Get-ChildItem -LiteralPath $base -Recurse -File) {
    $rel=$file.FullName.Substring($basePrefix.Length).Replace('\','/')
    if ($rel -in @('Assets/chisburner/common/chisburner66.elf',
                   'Assets/chisburner/common/chisburner66.ini')) { continue }
    $dest=Join-Path $sd $rel
    New-Item -ItemType Directory -Force (Split-Path $dest -Parent) | Out-Null
    Copy-Item -LiteralPath $file.FullName -Destination $dest
}

$common=Join-Path $sd 'Assets/chisburner/common'
$coreRoot=Join-Path $sd 'Cores/ChisBurner.ChisBurner'
Copy-Item -LiteralPath $os -Destination (Join-Path $common 'os.bin') -Force
Copy-Item -LiteralPath $app -Destination (Join-Path $common 'chisburner67.elf')
[IO.File]::WriteAllText((Join-Path $common 'chisburner67.ini'),
    "[os]`nELF=chisburner67.elf`nARGS=`n",$utf8)

$corePath=Join-Path $coreRoot 'core.json'
$core=Get-Content -Raw -LiteralPath $corePath | ConvertFrom-Json
$core.core.metadata.version='0.1.0-smoke.67-target-ident-readonly-diag'
$core.core.metadata.date_release='2026-09-05'
$core.core.metadata.description='Read-only target identify diagnostics; no erase/program UI'
[IO.File]::WriteAllText($corePath,($core|ConvertTo-Json -Depth 25),$utf8)

$dataPath=Join-Path $coreRoot 'data.json'
$data=Get-Content -Raw -LiteralPath $dataPath | ConvertFrom-Json
foreach ($slot in $data.data.data_slots) {
    switch ([int]$slot.id) {
        2 { $slot.filename='chisburner67.ini' }
        3 { $slot.filename='chisburner67.elf' }
        4 {
            $slot.name='Unused in read-only diagnostic'
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
    "smoke.67 read-only target-ident diagnostics; no unlock, erase, or program calls.`n",$utf8)

foreach ($json in Get-ChildItem -LiteralPath $sd -Filter '*.json' -Recurse -File) {
    $null=Get-Content -Raw -LiteralPath $json.FullName | ConvertFrom-Json
}
if (Get-ChildItem -LiteralPath $sd -Recurse -File | Where-Object Extension -eq '.gba') {
    throw 'Diagnostic package must not contain a ROM image'
}
if ((Sha (Join-Path $coreRoot 'chis.rbf_r')) -ne
    'fe73198c5e9164d9760ea62fd02ba671022b1083b028f99c62ca1cd949eca842') {
    throw 'Diagnostic package must reuse the timing-passing smoke66 RBF exactly'
}

Copy-Item -LiteralPath (Join-Path $root 'app/gba_program67_diag/README_TEST67_CN.txt') `
    -Destination (Join-Path $bundle 'README_TEST67_CN.txt')
[IO.File]::WriteAllLines((Join-Path $bundle 'UI_FIXTURES_SYNTHETIC.txt'),[string[]]$ui,$utf8)

$proof=@"
smoke=67-target-ident-readonly-diag
purpose=diagnose smoke66 TARGET_IDENTIFY_FAIL_RC_-10 after verified full-copy cold boot
application_calls=cart_get_caps;query_info;ping;flash_get_caps;safe_idle;identify_read;flash_get_caps_retry;safe_idle
application_never_calls=arm;erase_sector;program;output_write
source_rom_required=NO
fpga=byte-identical timing-passing smoke66 RBF
fpga_rbf_r_sha256=$(Sha (Join-Path $coreRoot 'chis.rbf_r'))
os_sha256=$(Sha (Join-Path $common 'os.bin'))
os_internal_crc=$('{0:x8}' -f $osCrc.ComputedCrc)
app_sha256=$(Sha (Join-Path $common 'chisburner67.elf'))
cart_caps_abi=v5 read-only raw diagnostic suffix; v3/v4 prefix negotiation retained
host_api_test=PASS
app_host_asan_ubsan=PASS
app_rv32_build=PASS
ui_pages=$pages;max_columns=$maxWidth;max_rows=$maxRows
"@
[IO.File]::WriteAllText((Join-Path $bundle 'BUILD_PROVENANCE.txt'),$proof,$utf8)

$sdPrefix=[IO.Path]::GetFullPath($sd).TrimEnd('\')+'\'
$hashes=@{}
$sums=Get-ChildItem -LiteralPath $sd -Recurse -File|Sort-Object FullName|ForEach-Object{
    $rel=$_.FullName.Substring($sdPrefix.Length).Replace('\','/')
    $hash=Sha $_.FullName
    $hashes["SD_ROOT/$rel"]=$hash
    "$hash  $rel"
}
[IO.File]::WriteAllLines((Join-Path $bundle 'SHA256SUMS.txt'),[string[]]$sums,$utf8)
Compress-Archive -Path (Join-Path $bundle '*') -DestinationPath $zip -CompressionLevel Optimal
$null=Assert-ChisBurnerZipOsIntegrity -ZipPath $zip

Add-Type -AssemblyName System.IO.Compression.FileSystem
$archive=[IO.Compression.ZipFile]::OpenRead($zip)
try {
    if ($archive.Entries|Where-Object{$_.FullName.Replace('\','/') -match '\.gba$'}) {
        throw 'ZIP contains a forbidden ROM image'
    }
    foreach ($name in $hashes.Keys) {
        $entry=$archive.GetEntry($name)
        if (!$entry) { throw "ZIP missing $name" }
        $stream=$entry.Open();$sha=[Security.Cryptography.SHA256]::Create()
        try {
            $actual=[BitConverter]::ToString($sha.ComputeHash($stream)).Replace('-','').ToLowerInvariant()
            if ($actual -ne $hashes[$name]) { throw "ZIP mismatch $name" }
        } finally { $stream.Dispose();$sha.Dispose() }
    }
} finally { $archive.Dispose() }

"PASS: $zip"
"ZIP SHA256: $(Sha $zip)"
"No ROM image; no erase/program application calls; $($hashes.Count) SD files verified"
