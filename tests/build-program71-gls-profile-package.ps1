[CmdletBinding()]
param(
    [string]$Distro='Ubuntu-24.04',
    [string]$OpenFpgaSdk='/home/lin13/work/ONS_FPGA/openfpgaSDK',
    [string]$FpgaBuild='.integration/openfpgaos/src/fpga/targets/pocket/bld/chisburner-program65-s1-route-maxfit-gls71-win'
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
$bundle=Join-Path $root 'build/smoke/ChisBurner-smoke.71-gls-profile-readonly'
$zip="$bundle.zip"
$fpga=if([IO.Path]::IsPathRooted($FpgaBuild)){$FpgaBuild}else{Join-Path $root $FpgaBuild}
$rawRbf=Join-Path $fpga 'output_files/ap_core.rbf'
$staPath=Join-Path $fpga 'output_files/ap_core.sta.rpt'
$qsfPath=Join-Path $fpga 'ap_core.qsf'
$fpgaHashes=Join-Path $fpga 'PROGRAM65_SHA256.txt'
$app=Join-Path $root 'build/program71-gls-profile-app/app.elf'
$utf8=[Text.UTF8Encoding]::new($false)
$baseZipHash='e3d2213463311a9e6cc140a5329dee7c165a3ded3c52c076486fc6ee432506d2'
$osHash='8d2f03f6a32584c6bd6c2a6fc87242928870aa248d83cb7afc2f7ae85b3c5662'

if ((Test-Path -LiteralPath $bundle) -or (Test-Path -LiteralPath $zip)) {
    throw 'Preserve existing smoke71 package'
}
foreach ($path in @($base,$rawRbf,$staPath,$qsfPath,$fpgaHashes)) {
    if (!(Test-Path -LiteralPath $path)) { throw "Missing package input $path" }
}
if ((Sha "$baseBundle.zip") -ne $baseZipHash -or
    (Sha (Join-Path $base 'Assets/chisburner/common/os.bin')) -ne $osHash) {
    throw 'Wrong smoke66 base or known-good OS'
}
foreach ($line in Get-Content -LiteralPath $fpgaHashes) {
    if ($line -notmatch '^([0-9a-f]{64})  (.+)$' -or
        !(Test-Path -LiteralPath $Matches[2]) -or
        (Sha $Matches[2]) -cne $Matches[1]) {
        throw "FPGA build input changed after compile: $line"
    }
}
$qsf=Get-Content -Raw -LiteralPath $qsfPath
if ($qsf -notmatch '(?m)^set_global_assignment -name VERILOG_MACRO CHISBURNER_GBA_PROGRAM65\s*$') {
    throw 'FPGA build is not the isolated programmer profile'
}
$sta=Get-Content -Raw -LiteralPath $staPath
$slacks=@{}
foreach ($kind in @('setup','hold')) {
    $matches=[regex]::Matches($sta,"Worst-case $kind slack is\s+(-?\d+(?:\.\d+)?)")
    if (!$matches.Count) { throw "Missing $kind timing result" }
    $slack=($matches|ForEach-Object{[double]$_.Groups[1].Value}|Measure-Object -Minimum).Minimum
    if ($slack -le 0) { throw "Refuse package: $kind timing failed $slack ns" }
    $slacks[$kind]=$slack
}

$wrapper=Get-Content -Raw -LiteralPath (Join-Path $root 'app/gba_program71_gls_profile/main.c')
$implementation=Get-Content -Raw -LiteralPath (Join-Path $root 'app/gba_program69_raw_ident/main.c')
foreach ($required in @('D69_SPACE_ADDRESS 0x4000015c','D69_EXPECTED_BUFFER_EXP 0x09')) {
    if ($wrapper -notmatch [regex]::Escape($required)) { throw "Missing smoke71 contract: $required" }
}
foreach ($forbidden in @('of_cart_flash_arm','of_cart_flash_erase_sector','of_cart_flash_program',
                          'D69_REG_TX_DATA','D69_REG_ARM','CMD_ERASE','CMD_PROGRAM')) {
    if (($wrapper+$implementation).Contains($forbidden)) { throw "Forbidden write primitive: $forbidden" }
}

Wsl "cd '$wroot' && bash tests/run-program65-periph-rtl.sh"
Wsl "cd '$wroot' && verilator --cc --exe --build -j 4 -Wall -Wno-fatal --top-module chisburner_cart_gba_s29gl256 -Mdir build/program71-package-rtl .integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gba_readonly.v .integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gba_s29gl256.v '$wroot/tests/test-program65-rtl.cpp' && build/program71-package-rtl/Vchisburner_cart_gba_s29gl256"
Wsl "cd '$wroot/app/gba_program71_gls_profile' && make host-test all OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0"
Wsl "cd '$wroot' && cc -DOF_PC -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined -fno-omit-frame-pointer -I.integration/openfpgaos/src/firmware/api app/gba_program71_gls_profile/main.c -o build/program71-gls-profile-app/pc-ui-san && build/program71-gls-profile-app/pc-ui-san >/dev/null"

$elf=[IO.File]::ReadAllBytes($app)
if ($elf[0] -ne 0x7f -or $elf[4] -ne 1 -or $elf[5] -ne 1 -or
    [BitConverter]::ToUInt16($elf,18) -ne 243 -or
    ![Text.Encoding]::ASCII.GetString($elf).Contains('smoke.71')) {
    throw 'Wrong smoke71 application ELF'
}
$load=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && riscv64-unknown-elf-readelf -l build/program71-gls-profile-app/app.elf")-join "`n"
if ($LASTEXITCODE -ne 0 -or
    $load -notmatch 'LOAD\s+0x[0-9a-f]+\s+0x10400000\s+0x10400000' -or
    $load -match 'INTERP|DYNAMIC') { throw 'Unexpected application load map' }

$ui=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && build/program71-gls-profile-app/pc-ui")
if ($LASTEXITCODE -ne 0) { throw 'UI fixture failed' }
$ui=$ui|ForEach-Object{[regex]::Replace([string]$_,'\x1b\[[0-9;]*[A-Za-z]','')}
$pages=0;$rows=0;$maxRows=0;$maxWidth=0
foreach ($line in $ui) {
    if ($line -match '^ GBA RAW IDENT smoke\.71 [1-4]/4$') {
        $maxRows=[Math]::Max($maxRows,$rows);$rows=0;++$pages
    }
    ++$rows;$maxWidth=[Math]::Max($maxWidth,$line.Length)
    if ($line.Length -gt 40) { throw "UI line too wide: $line" }
}
$maxRows=[Math]::Max($maxRows,$rows)
if ($pages -ne 4 -or $maxRows -gt 18 -or
    !($ui -contains 'PUBLIC FIXED CHECKS: PASS') -or
    !($ui -contains 'HARDWARE PROFILE: PASS')) {
    throw "UI contract failed pages=$pages rows=$maxRows"
}

if (-not ('ChisBurner.Program71.Reverse' -as [type])) {
    Add-Type -TypeDefinition @'
namespace ChisBurner.Program71 {
    public static class Reverse {
        public static byte[] Apply(byte[] input) {
            var table = new byte[256];
            for (int i = 0; i < 256; ++i) {
                int x = i, y = 0;
                for (int bit = 0; bit < 8; ++bit) { y = (y << 1) | (x & 1); x >>= 1; }
                table[i] = (byte)y;
            }
            var output = new byte[input.Length];
            for (int i = 0; i < input.Length; ++i) output[i] = table[input[i]];
            return output;
        }
    }
}
'@
}
$raw=[IO.File]::ReadAllBytes($rawRbf)
$rbfR=[ChisBurner.Program71.Reverse]::Apply($raw)
$roundtrip=[ChisBurner.Program71.Reverse]::Apply($rbfR)
if ([BitConverter]::ToString([ChisBurner.Program71.Reverse]::Apply([byte[]](1,128,18,170))) -ne '80-01-48-55' -or
    ![Linq.Enumerable]::SequenceEqual([byte[]]$raw,[byte[]]$roundtrip)) {
    throw 'RBF bit reversal failed'
}

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
Copy-Item -LiteralPath $app -Destination (Join-Path $common 'chisburner71.elf')
[IO.File]::WriteAllText((Join-Path $common 'chisburner71.ini'),"[os]`nELF=chisburner71.elf`nARGS=`n",$utf8)
[IO.File]::WriteAllBytes((Join-Path $coreRoot 'chis.rbf_r'),$rbfR)
if ((Sha (Join-Path $common 'os.bin')) -ne $osHash) { throw 'Known-good OS changed' }
$osCrc=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes((Join-Path $common 'os.bin')))

$corePath=Join-Path $coreRoot 'core.json'
$core=Get-Content -Raw -LiteralPath $corePath | ConvertFrom-Json
$core.core.metadata.version='0.1.0-smoke.71-gls-profile-readonly'
$core.core.metadata.date_release='2026-09-05'
$core.core.metadata.description='Read-only S29GL256S CFI profile verification'
[IO.File]::WriteAllText($corePath,($core|ConvertTo-Json -Depth 25),$utf8)
$dataPath=Join-Path $coreRoot 'data.json'
$data=Get-Content -Raw -LiteralPath $dataPath | ConvertFrom-Json
foreach ($slot in $data.data.data_slots) {
    switch ([int]$slot.id) {
        2 { $slot.filename='chisburner71.ini' }
        3 { $slot.filename='chisburner71.elf' }
        4 {
            $slot.name='Unused in GL-S profile read-only test';$slot.required=$false
            $slot.parameters=0;$slot.extensions=@('bin');$slot.deferload=$true
            foreach ($property in @('filename','nonvolatile','address','size_exact','size_maximum','secondary')) {
                $slot.PSObject.Properties.Remove($property)
            }
        }
    }
}
[IO.File]::WriteAllText($dataPath,($data|ConvertTo-Json -Depth 25),$utf8)
[IO.File]::WriteAllText((Join-Path $coreRoot 'info.txt'),
    "smoke.71 read-only GL-S CFI profile: 32MiB, 128KiB sectors, 512-byte maximum buffer; conservative program chunk remains 32 bytes.`n",$utf8)
foreach ($json in Get-ChildItem -LiteralPath $sd -Filter '*.json' -Recurse -File) {
    $null=Get-Content -Raw -LiteralPath $json.FullName | ConvertFrom-Json
}
if (Get-ChildItem -LiteralPath $sd -Recurse -File | Where-Object Extension -eq '.gba') {
    throw 'Diagnostic package must not contain a ROM image'
}

Copy-Item -LiteralPath (Join-Path $root 'app/gba_program71_gls_profile/README_TEST71_CN.txt') -Destination $bundle
Copy-Item -LiteralPath $fpgaHashes -Destination (Join-Path $bundle 'FPGA_BUILD_SOURCE_SHA256.txt')
[IO.File]::WriteAllLines((Join-Path $bundle 'UI_FIXTURES_SYNTHETIC.txt'),[string[]]$ui,$utf8)
$proof=@"
smoke=71-gls-profile-readonly
target=S29GL256S-compatible;32MiB;128KiB sectors;CFI buffer exponent 09h
actual_program_chunk=32 bytes;not 512 bytes
application_commands=SAFE_IDLE;READ_IDENTIFY;SAFE_IDLE
application_never_commands=ARM;ERASE;PROGRAM
source_rom_required=NO
known_good_os_sha256=$(Sha (Join-Path $common 'os.bin'))
os_internal_crc=$('{0:x8}' -f $osCrc.ComputedCrc)
fpga_rbf_raw_sha256=$(Sha $rawRbf)
fpga_rbf_r_sha256=$(Sha (Join-Path $coreRoot 'chis.rbf_r'))
app_sha256=$(Sha (Join-Path $common 'chisburner71.elf'))
setup_slack_ns=$($slacks['setup'])
hold_slack_ns=$($slacks['hold'])
rtl_profile_accept_09=PASS
rtl_profile_reject_05=PASS
peripheral_mapping=PASS
app_host_asan_ubsan=PASS
app_rv32_build=PASS
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
"GL-S profile read only; new RBF + exact smoke66 OS; $($hashes.Count) SD files verified"
