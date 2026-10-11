[CmdletBinding()]
param(
    [string]$Distro='Ubuntu-24.04',
    [string]$OpenFpgaSdk='/home/lin13/work/ONS_FPGA/openfpgaSDK',
    [string]$OsBuild='.integration/openfpgaos/src/firmware/os/bld/program66-pocket-release',
    [string]$FpgaBuild='.integration/openfpgaos/src/fpga/targets/pocket/bld/chisburner-program65-s1-route-maxfit-capfix66-win'
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
$baseBundle=Join-Path $root 'build/smoke/ChisBurner-smoke.64-dynamic-emerald32-fastdump-cn'
$base=Join-Path $baseBundle 'SD_ROOT'
$bundle=Join-Path $root 'build/smoke/ChisBurner-smoke.66-s29gl256-programmer-preview'
$zip="$bundle.zip"
$fpga=if([IO.Path]::IsPathRooted($FpgaBuild)){$FpgaBuild}else{Join-Path $root $FpgaBuild}
$rawRbf=Join-Path $fpga 'output_files/ap_core.rbf'
$staPath=Join-Path $fpga 'output_files/ap_core.sta.rpt'
$qsfPath=Join-Path $fpga 'ap_core.qsf'
$fpgaHashes=Join-Path $fpga 'PROGRAM65_SHA256.txt'
$osRoot=if([IO.Path]::IsPathRooted($OsBuild)){$OsBuild}else{Join-Path $root $OsBuild}
$os=Join-Path $osRoot 'os.bin'
$osManifest=Join-Path $osRoot 'PROGRAM65_OS_SHA256.txt'
$app=Join-Path $root 'build/program65-app/app.elf'
$utf8=[Text.UTF8Encoding]::new($false)

if ((Test-Path -LiteralPath $bundle) -or (Test-Path -LiteralPath $zip)) {
    throw 'Preserve existing smoke66 package'
}
foreach ($path in @($base,$rawRbf,$staPath,$qsfPath,$fpgaHashes,$os,$osManifest)) {
    if (!(Test-Path -LiteralPath $path)) { throw "Missing package input $path" }
}
foreach ($line in Get-Content -LiteralPath $osManifest) {
    if ($line -match '^EXTRA_CFLAGS=') {
        foreach ($flag in @('CART_MMIO_ENABLED=1','CART_S29GL256_PROGRAMMER=1',
                            'CHISBURNER_OUTPUT_ENABLED=1')) {
            if ($line -notmatch [regex]::Escape($flag)) {
                throw "PROGRAM65 OS missing $flag"
            }
        }
        continue
    }
    if ($line -notmatch '^([0-9a-f]{64})  (.+)$' -or
        !(Test-Path -LiteralPath $Matches[2]) -or
        (Sha $Matches[2]) -cne $Matches[1]) {
        throw "PROGRAM65 OS source drift: $line"
    }
}
foreach ($line in Get-Content -LiteralPath (Join-Path $baseBundle 'SHA256SUMS.txt')) {
    if ($line -notmatch '^([0-9a-f]{64})  (.+)$') {
        throw 'Malformed smoke64 base manifest'
    }
    if ((Sha (Join-Path $base $Matches[2])) -cne $Matches[1]) {
        throw "Changed smoke64 base payload: $($Matches[2])"
    }
}
foreach ($line in Get-Content -LiteralPath $fpgaHashes) {
    if ($line -notmatch '^([0-9a-f]{64})  (.+)$') {
        throw 'Malformed PROGRAM65 source manifest'
    }
    if (!(Test-Path -LiteralPath $Matches[2]) -or
        (Sha $Matches[2]) -cne $Matches[1]) {
        throw "FPGA build input changed after compile: $($Matches[2])"
    }
}

$qsf=Get-Content -Raw -LiteralPath $qsfPath
if ($qsf -notmatch '(?m)^set_global_assignment -name VERILOG_MACRO CHISBURNER_GBA_PROGRAM65\s*$' -or
    $qsf -match '(?m)^set_global_assignment -name VERILOG_MACRO CHISBURNER_GBA_IDENT56\s*$' -or
    $qsf -match '(?m)^set_global_assignment -name VERILOG_MACRO CHISBURNER_STORAGE_(TRACE|EVENTS|AUDIT)\s*$') {
    throw 'FPGA build is not the isolated PROGRAM65 profile'
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

Wsl "cd '$wroot/app/gba_program65' && make host-test all OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0"
Wsl "cd '$wroot' && cc -DOF_PC -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined -fno-omit-frame-pointer -I.integration/openfpgaos/src/firmware/api app/gba_program65/main.c -o build/program65-app/pc-ui-san && build/program65-app/pc-ui-san >/dev/null"
Wsl "cd '$wroot' && verilator --cc --exe --build -j 4 -Wall -Wno-fatal --top-module chisburner_cart_gba_s29gl256 -Mdir build/program65-rtl .integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gba_readonly.v .integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gba_s29gl256.v '$wroot/tests/test-program65-rtl.cpp' && build/program65-rtl/Vchisburner_cart_gba_s29gl256"
Wsl "cd '$wroot' && bash tests/run-program65-periph-rtl.sh"

$elf=[IO.File]::ReadAllBytes($app)
if ($elf[0] -ne 0x7f -or $elf[4] -ne 1 -or $elf[5] -ne 1 -or
    [BitConverter]::ToUInt16($elf,18) -ne 243 -or
    ![Text.Encoding]::ASCII.GetString($elf).Contains('GBA PROGRAM smoke.66')) {
    throw 'Wrong application ELF'
}
$load=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && riscv64-unknown-elf-readelf -l build/program65-app/app.elf")-join "`n"
if ($LASTEXITCODE -ne 0 -or
    $load -notmatch 'LOAD\s+0x[0-9a-f]+\s+0x10400000\s+0x10400000' -or
    $load -match 'INTERP|DYNAMIC') {
    throw 'Unexpected application load map'
}

$ui=@(& wsl.exe -d $Distro -- bash -lc "cd '$wroot' && build/program65-app/pc-ui")
if ($LASTEXITCODE -ne 0) { throw 'UI fixture failed' }
$ui=$ui|ForEach-Object{[regex]::Replace([string]$_,'\x1b\[[0-9;]*[A-Za-z]','')}
$pages=0;$rows=0;$maxRows=0;$maxWidth=0
foreach ($line in $ui) {
    if ($line -eq ' GBA PROGRAM smoke.66') {
        $maxRows=[Math]::Max($maxRows,$rows);$rows=0;++$pages
    }
    ++$rows
    $maxWidth=[Math]::Max($maxWidth,$line.Length)
    if ($line.Length -gt 40) { throw "UI line too wide: $line" }
}
$maxRows=[Math]::Max($maxRows,$rows)
if ($pages -ne 11 -or $maxRows -gt 18) {
    throw "UI shape failed pages=$pages rows=$maxRows"
}

$osCrc=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes($os))
if (-not ('ChisBurner.Program65.Reverse' -as [type])) {
    Add-Type -TypeDefinition @'
namespace ChisBurner.Program65 {
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
if ([BitConverter]::ToString([ChisBurner.Program65.Reverse]::Apply([byte[]](1,128,18,170))) -ne '80-01-48-55') {
    throw 'RBF bit-reversal vector failed'
}
$raw=[IO.File]::ReadAllBytes($rawRbf)
$rbfR=[ChisBurner.Program65.Reverse]::Apply($raw)
$roundtrip=[ChisBurner.Program65.Reverse]::Apply($rbfR)
if ($roundtrip.Length -ne $raw.Length) { throw 'RBF round-trip length changed' }
for ($i=0;$i -lt $raw.Length;++$i) {
    if ($roundtrip[$i] -ne $raw[$i]) { throw "RBF round-trip failed at $i" }
}

$sd=Join-Path $bundle 'SD_ROOT'
New-Item -ItemType Directory -Path $bundle,$sd | Out-Null
$omit=@(
    'Assets/chisburner/common/chisburner64.elf',
    'Assets/chisburner/common/chisburner64.ini',
    'Saves/chisburner/common/DYNAMIC64-README.txt'
)
$basePrefix=[IO.Path]::GetFullPath($base).TrimEnd('\')+'\'
foreach ($file in Get-ChildItem -LiteralPath $base -Recurse -File) {
    $rel=$file.FullName.Substring($basePrefix.Length).Replace('\','/')
    if ($rel -in $omit) { continue }
    $dest=Join-Path $sd $rel
    New-Item -ItemType Directory -Force (Split-Path $dest -Parent) | Out-Null
    Copy-Item -LiteralPath $file.FullName -Destination $dest
}

$common=Join-Path $sd 'Assets/chisburner/common'
$coreRoot=Join-Path $sd 'Cores/ChisBurner.ChisBurner'
Copy-Item -LiteralPath $os -Destination (Join-Path $common 'os.bin') -Force
Copy-Item -LiteralPath $app -Destination (Join-Path $common 'chisburner66.elf')
[IO.File]::WriteAllText((Join-Path $common 'chisburner66.ini'),
    "[os]`nELF=chisburner66.elf`nARGS=`n",$utf8)
[IO.File]::WriteAllBytes((Join-Path $coreRoot 'chis.rbf_r'),$rbfR)

$corePath=Join-Path $coreRoot 'core.json'
$core=Get-Content -Raw -LiteralPath $corePath | ConvertFrom-Json
$core.core.metadata.version='0.1.0-smoke.66-s29gl256-programmer-preview'
$core.core.metadata.date_release='2026-09-05'
$core.core.metadata.description='Safety-gated S29GL256N GBA NOR programmer preview'
$core.core.framework.version_required='2.3'
[IO.File]::WriteAllText($corePath,($core|ConvertTo-Json -Depth 25),$utf8)

$dataPath=Join-Path $coreRoot 'data.json'
$data=Get-Content -Raw -LiteralPath $dataPath | ConvertFrom-Json
foreach ($slot in $data.data.data_slots) {
    switch ([int]$slot.id) {
        2 { $slot.filename='chisburner66.ini' }
        3 { $slot.filename='chisburner66.elf' }
        4 {
            $slot.name='32 MiB GBA ROM TO PROGRAM'
            $slot.required=$true
            $slot.parameters=2
            $slot.deferload=$true
            $slot.extensions=@('gba')
            $slot | Add-Member -NotePropertyName size_exact -NotePropertyValue 33554432 -Force
            foreach ($property in @('filename','nonvolatile','address','size_maximum','secondary')) {
                $slot.PSObject.Properties.Remove($property)
            }
        }
    }
}
[IO.File]::WriteAllText($dataPath,($data|ConvertTo-Json -Depth 25),$utf8)
[IO.File]::WriteAllText((Join-Path $coreRoot 'info.txt'),
    "smoke.66 preview: fixed GBA capability map; exact S29GL256N CFI gate, 128 KiB sector erase, 32-byte buffered program, final cartridge MD5.`n",$utf8)

$slot4=@($data.data.data_slots|Where-Object id -eq 4)
if ($slot4.Count -ne 1 -or !$slot4[0].required -or !$slot4[0].deferload -or
    [int]$slot4[0].parameters -ne 2 -or [int64]$slot4[0].size_exact -ne 33554432 -or
    $slot4[0].filename) {
    throw 'Source slot 4 is not required/read-only/exact-32MiB/unbound'
}
foreach ($json in Get-ChildItem -LiteralPath $sd -Filter '*.json' -Recurse -File) {
    $null=Get-Content -Raw -LiteralPath $json.FullName | ConvertFrom-Json
}
if (Get-ChildItem -LiteralPath $sd -Recurse -File | Where-Object Extension -eq '.gba') {
    throw 'Package must not contain a ROM image'
}

Copy-Item -LiteralPath (Join-Path $root 'app/gba_program65/README_TEST65_CN.txt') `
    -Destination (Join-Path $bundle 'README_TEST66_CN.txt')
Copy-Item -LiteralPath $fpgaHashes -Destination (Join-Path $bundle 'FPGA_BUILD_SOURCE_SHA256.txt')
[IO.File]::WriteAllLines((Join-Path $bundle 'UI_FIXTURES_SYNTHETIC.txt'),[string[]]$ui,$utf8)

$sources=@(
    'app/gba_program65/main.c','app/gba_program65/program65.h',
    'app/gba_program65/test_program65.c','app/gba_program65/Makefile',
    'app/gba_program65/README_TEST65_CN.txt','app/storage_stream52/gate.h',
    '.integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gba_readonly.v',
    '.integration/openfpgaos/src/fpga/targets/pocket/chisburner_cart_gba_s29gl256.v',
    '.integration/openfpgaos/src/fpga/targets/pocket/core_top.v',
    '.integration/openfpgaos/src/fpga/common/axi_periph_slave.v',
    '.integration/openfpgaos/src/firmware/os/.contract.md5',
    'core/storage/pct_bridge_audit.v',
    '.integration/openfpgaos/src/firmware/api/of_cart.h',
    '.integration/openfpgaos/src/firmware/api/of_services.h',
    '.integration/openfpgaos/src/firmware/os/hal/regs.h',
    '.integration/openfpgaos/src/firmware/os/kernel/services_table.c',
    'tests/test-program65-rtl.cpp','tests/rtl/tb_program65_periph.cpp',
    'tests/run-program65-periph-rtl.sh','tests/build-program65-os.ps1',
    'tests/build-program65-fpga.ps1',
    'tests/report-program65-allcorners.tcl',
    'tests/build-program65-package.ps1','docs/dump-roadmap.md'
)
[IO.File]::WriteAllLines((Join-Path $bundle 'SOFTWARE_SOURCE_SHA256.txt'),[string[]]@(
    $sources|ForEach-Object{"$(Sha (Join-Path $root $_))  $_"}
),$utf8)

$proof=@"
smoke=66-s29gl256-programmer-preview
hardware_result=PENDING_FIRST_CONTROLLED_WRITE_AFTER_CAPABILITY_MAP_FIX
smoke65_result=TARGET_IDENTIFY_FAIL_RC_-10_BEFORE_CFI;NO_ERASE_OR_WRITE
capability_contract=rev4;GB_spaces_bits_16_20;GBA_spaces_bits_21_25;nonoverlap_compile_guard
scope=only exact CFI-qualified S29GL256N-compatible 32MiB NOR;128KiB sectors;32-byte write buffer
source_slot=4;required;read-only;unbound;exact 33554432 bytes;gba extension
source_rom_shipped=NO
source_preflight=exact size;Nintendo logo SHA1;GBA header checksum;fixed 0x96;streaming MD5
ordering=target identify;source full scan;physical confirmation;target re-identify;erase;program;final cart MD5
authorization=release 0.5s;hold L+R 2s;release 0.5s;A;release 0.5s;hold START 1s
erase=256 x 128KiB sector erase;no chip erase command
program=1048576 x 32-byte write-buffer operations
polling=GL-N DQ7;DQ5 recheck;DQ1 write-buffer-abort recovery
verification=source MD5;program-stream MD5;one final full cartridge MD5
per_block_readback=NO
verify_cartridge_byte_compare=DEFERRED_TODO
asynchronous_cartridge_pipeline=DEFERRED_2_0_OR_3_0_TODO
save_rtc_access=NO
setup_slack_ns=$($slacks['setup'])
hold_slack_ns=$($slacks['hold'])
quartus_build=$([IO.Path]::GetFileName($fpga))
rbf_raw_sha256=$(Sha $rawRbf)
rbf_r_sha256=$(Sha (Join-Path $coreRoot 'chis.rbf_r'))
os_sha256=$(Sha (Join-Path $common 'os.bin'))
os_internal_crc=$('{0:x8}' -f $osCrc.ComputedCrc)
app_sha256=$(Sha (Join-Path $common 'chisburner66.elf'))
app_host_fault_gates=ASan_UBSan_PASS
programmer_rtl_protocol=PASS
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
"Timing: setup $($slacks['setup']) ns; hold $($slacks['hold']) ns"
"No ROM image included; $($hashes.Count) SD payload files verified"
