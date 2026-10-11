[CmdletBinding()]
param([string]$Distro = 'Ubuntu-24.04',
      [string]$OpenFpgaSdk = '/home/lin13/work/ONS_FPGA/openfpgaSDK',
      [string]$Version = '0.1.0-smoke.46-host-events')
$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'storage-event-service-patch.ps1')
function Get-Sha([string]$Path) { (Get-FileHash -LiteralPath $Path).Hash.ToLowerInvariant() }
function Wsl-Path([string]$Path) {
    $full = [IO.Path]::GetFullPath($Path)
    if ($full -notmatch '^([A-Za-z]):\\(.*)$') { throw 'Expected local drive path' }
    '/mnt/' + $Matches[1].ToLowerInvariant() + '/' + $Matches[2].Replace('\','/')
}
if ($Version -notmatch '^0\.1\.0-smoke\.46-[a-z0-9-]+$' -or $Version.Length -gt 31) { throw 'Invalid version' }
$root = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$baseSd = Join-Path $root 'build\smoke\ChisBurner-storage-flush-0.1.0-smoke.44-host-trace\SD_ROOT'
$fpga = Join-Path $root '.integration\openfpgaos\src\fpga\targets\pocket\bld\chisburner-storage-events-s46-win'
$bundle = Join-Path $root "build\smoke\ChisBurner-storage-flush-$Version"
$zipPath = "$bundle.zip"
if ((Test-Path -LiteralPath $bundle) -or (Test-Path -LiteralPath $zipPath)) { throw 'Preserve existing package' }
$osRel = 'Assets\chisburner\common\os.bin'
$appRel = 'Assets\chisburner\common\chisburnersmoke.elf'
$rbfRel = 'Cores\ChisBurner.ChisBurner\chis.rbf_r'
$coreRel = 'Cores\ChisBurner.ChisBurner\core.json'
$oldInstance = 'Assets\chisburner\ChisBurner.ChisBurner\TF Flush TRACE 44.json'
$newInstance = 'Assets\chisburner\ChisBurner.ChisBurner\TF Trace 46.json'
$seedRel = 'Assets\chisburner\common\storage-seed46.bin'
$critical = @{
    $osRel = '45792a4fe6937403fd73a5efd1d4077f65e6c62454aa1100f851c9c2b80d4e1a'
    $rbfRel = '9f75bc14165687901fc2e777c2a8ec0a7c655ba6ada146586f53215e3f232978'
    $appRel = '34b808b223e5db02edf12e4550130e528dbfaf2ee4827a7e1b56c11cf523b770'
}
foreach ($rel in $critical.Keys) {
    if ((Get-Sha (Join-Path $baseSd $rel)) -ne $critical[$rel]) { throw "Base mismatch: $rel" }
}
$qsf = Get-Content -Raw -LiteralPath (Join-Path $fpga 'ap_core.qsf')
foreach ($macro in @('CHISBURNER_STORAGE_TRACE','CHISBURNER_STORAGE_EVENTS')) {
    if ($qsf -notmatch "(?m)^set_global_assignment -name VERILOG_MACRO $macro\s*$") { throw "Missing $macro" }
}
if ($qsf -match '(?m)^set_global_assignment -name VERILOG_MACRO INCLUDE_CARTRIDGE\s*$') { throw 'Wrong cartridge profile' }
foreach ($stage in @('map','fit','asm','mif_update')) {
    $rpt = Get-Content -Raw -LiteralPath (Join-Path $fpga "output_files\ap_core.$stage.rpt")
    if ($rpt -notmatch 'was successful\. 0 errors') { throw "FPGA $stage did not pass" }
}
$mif = Get-Content -Raw -LiteralPath (Join-Path $fpga 'output_files\ap_core.mif_update.rpt')
if ($mif -notmatch 'Processed Memory Initialization File .*firmware\.mif' -or
    $mif -notmatch 'Processed Memory Initialization File .*build_id\.mif' -or
    $mif -match 'Critical Warning|can.t find|setting all initial values to 0') { throw 'Unverified boot embedding' }
if ((Get-Sha (Join-Path $fpga 'firmware.mif')) -ne 'aade051a2a517f71d55f1610a3b4e6379e73896c68c322a96556f3982befbba2') { throw 'Boot changed' }
foreach ($line in Get-Content -LiteralPath (Join-Path $fpga 'SOURCE_SHA256.txt')) {
    if ($line -notmatch '^([0-9a-f]{64})  (.+)$') { throw 'Malformed source manifest' }
    $expected = $Matches[1]; $path = $Matches[2]
    if ((Get-Sha $path) -ne $expected) { throw "Source changed since compile: $path" }
}
$sta = Get-Content -Raw -LiteralPath (Join-Path $fpga 'output_files\ap_core.sta.rpt')
$slack = @{}
foreach ($kind in @('setup','hold')) {
    $m = [regex]::Matches($sta,"Worst-case $kind slack is\s+(-?\d+(?:\.\d+)?)")
    if (!$m.Count) { throw "Missing $kind slack" }
    $slack[$kind] = ($m | ForEach-Object { [double]$_.Groups[1].Value } | Measure-Object -Minimum).Minimum
    if ($slack[$kind] -le 0) { throw "Timing failed: $kind" }
}
$checks = @{
    'storage-events\test.log' = '827 PASS, 0 FAIL'
    'storage-events-periph\test.log' = '31 passed, 0 failed'
    'storage-buffered\test.log' = '154 PASS, 0 FAIL'
    'storage-trace\bridge-test.log' = '50 passed, 0 failed'
    'storage-trace\periph-test.log' = '15 passed, 0 failed'
}
foreach ($rel in $checks.Keys) {
    $log = Get-Content -Raw -LiteralPath (Join-Path $root "build\rtl\$rel")
    if (!$log.Contains($checks[$rel])) { throw "RTL test not passed: $rel" }
}
$appDir = Wsl-Path (Join-Path $root 'app\storage_event_probe')
$projectWsl = Wsl-Path $root
$source = (Get-Content -Raw -LiteralPath (Join-Path $root 'app\storage_event_probe\main.c')) +
          (Get-Content -Raw -LiteralPath (Join-Path $root 'app\storage_event_probe\probe.h')) +
          (Get-Content -Raw -LiteralPath (Join-Path $root 'app\storage_slot_probe\probe.h'))
if ($source -match 'of_cart_|0x40000|of_output_write\s*\(') { throw 'App must use SDK; no cartridge/output writes' }
& wsl.exe -d $Distro -- bash -lc "cd '$projectWsl' && mkdir -p build/storage-event-patch && riscv64-unknown-elf-as -march=rv32imafc -mabi=ilp32f tests/patches/smoke44-bridge-trace-service.S -o build/storage-event-patch/patch.o && riscv64-unknown-elf-objcopy -O binary -j .text build/storage-event-patch/patch.o build/storage-event-patch/patch.bin && c++ -std=c++17 -Wall -Wextra -Werror tests/test-storage-event-service.cpp -o build/storage-event-patch/check && build/storage-event-patch/check build/storage-event-patch/patch.bin && cd '$appDir' && make clean OPENFPGA_SDK='$OpenFpgaSdk' && make host-test all OPENFPGA_SDK='$OpenFpgaSdk' && cd ../storage_slot_probe && make host-test OPENFPGA_SDK='$OpenFpgaSdk'"
if ($LASTEXITCODE -ne 0) { throw 'Application or patch checks failed' }
$elf = Join-Path $root 'build\storage-event-app\app.elf'
$elfText = [Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($elf))
if (!$elfText.Contains('TF TRACE smoke.46') -or $elfText.Contains('TF SLOT smoke.45')) { throw 'ELF identity mismatch' }
$osBytes = Set-ChisBurnerEventServicePatch -Bytes ([IO.File]::ReadAllBytes((Join-Path $baseSd $osRel))) `
    -Instructions ([IO.File]::ReadAllBytes((Join-Path $root 'build\storage-event-patch\patch.bin')))
if ($osBytes.Length -ne 137480) { throw 'OS length changed' }
$crc = Assert-ChisBurnerOsIntegrity -Bytes $osBytes
New-Item -ItemType Directory -Path $bundle | Out-Null
Copy-Item -LiteralPath $baseSd -Destination $bundle -Recurse
$sd = Join-Path $bundle 'SD_ROOT'
$utf8 = New-Object Text.UTF8Encoding($false)
[IO.File]::WriteAllBytes((Join-Path $sd $osRel),$osBytes)
Copy-Item -LiteralPath $elf -Destination (Join-Path $sd $appRel)
# Move only the freshly copied launch descriptor in this new staging folder.
$resolvedSource = [IO.Path]::GetFullPath((Join-Path $sd $oldInstance))
$resolvedDest = [IO.Path]::GetFullPath((Join-Path $sd $newInstance))
$sdPrefix = [IO.Path]::GetFullPath($sd).TrimEnd('\') + '\'
if (!$resolvedSource.StartsWith($sdPrefix) -or !$resolvedDest.StartsWith($sdPrefix)) { throw 'Unsafe staging path' }
Move-Item -LiteralPath $resolvedSource -Destination $resolvedDest
$instance = Get-Content -Raw -LiteralPath $resolvedDest | ConvertFrom-Json
if (@($instance.instance.data_slots).Count -ne 3 -or @($instance.instance.data_slots | Where-Object id -eq 4).Count) { throw 'Unexpected base instance' }
$instance.instance.data_slots = @($instance.instance.data_slots) + @([pscustomobject]@{id=4;filename='storage-seed46.bin'})
[IO.File]::WriteAllText($resolvedDest,($instance | ConvertTo-Json -Depth 20),$utf8)
[IO.File]::WriteAllBytes((Join-Path $sd $seedRel),[byte[]](0..63))
$data = Get-Content -Raw -LiteralPath (Join-Path $sd 'Cores\ChisBurner.ChisBurner\data.json') | ConvertFrom-Json
$slot4 = @($data.data.data_slots | Where-Object id -eq 4)
if ($slot4.Count -ne 1 -or !$slot4[0].deferload -or ([int]$slot4[0].parameters -band 8)) { throw 'Slot must remain writable/deferred' }
$corePath = Join-Path $sd $coreRel
$core = Get-Content -Raw -LiteralPath $corePath | ConvertFrom-Json
if ([int64]$core.core.framework.hardware.cartridge_adapter -ne -1) { throw 'Cartridge must remain disabled' }
$core.core.metadata.version = $Version
$core.core.metadata.date_release = '2026-09-03'
[IO.File]::WriteAllText($corePath,($core | ConvertTo-Json -Depth 20),$utf8)
if (-not ('ChisBurner.Build.ReverseRbf' -as [type])) {
    Add-Type -TypeDefinition @'
namespace ChisBurner.Build {
    public static class ReverseRbf {
        public static byte[] Apply(byte[] input) {
            var table=new byte[256];
            for(int i=0;i<256;i++) { int x=i,y=0; for(int b=0;b<8;b++) { y=(y<<1)|(x&1); x>>=1; } table[i]=(byte)y; }
            var output=new byte[input.Length];
            for(int i=0;i<input.Length;i++) output[i]=table[input[i]];
            return output;
        }
    }
}
'@
}
$rawRbf = Join-Path $fpga 'output_files\ap_core.rbf'
$raw = [IO.File]::ReadAllBytes($rawRbf)
[IO.File]::WriteAllBytes((Join-Path $sd $rbfRel),[ChisBurner.Build.ReverseRbf]::Apply($raw))
if ([BitConverter]::ToString([ChisBurner.Build.ReverseRbf]::Apply([byte[]](0x01,0x80,0x12,0xAA))) -ne '80-01-48-55') { throw 'Bit reversal failed' }
$sha = [Security.Cryptography.SHA256]::Create()
try {
    $hash = [BitConverter]::ToString($sha.ComputeHash([ChisBurner.Build.ReverseRbf]::Apply([IO.File]::ReadAllBytes((Join-Path $sd $rbfRel))))).Replace('-','').ToLowerInvariant()
} finally { $sha.Dispose() }
if ($hash -ne (Get-Sha $rawRbf)) { throw 'RBF round trip failed' }
$basePrefix = [IO.Path]::GetFullPath($baseSd).TrimEnd('\') + '\'
foreach ($f in Get-ChildItem -LiteralPath $baseSd -Recurse -File) {
    $rel = $f.FullName.Substring($basePrefix.Length)
    if ($rel -in @($osRel,$appRel,$rbfRel,$coreRel,$oldInstance)) { continue }
    if ((Get-Sha $f.FullName) -ne (Get-Sha (Join-Path $sd $rel))) { throw "Unexpected payload change: $rel" }
}
$readme = @"
ChisBurner - smoke.46 host communication event trace

Keep the cartridge slot EMPTY. Merge ALL of SD_ROOT, including Saves, and
approve replacement. The old storage-smoke.41.bin diagnostic placeholder is
reset; no cartridge save is used. Power OFF/ON. Select "TF Trace 46".
Screen MUST say "TF TRACE smoke.46". Old entry names can remain after a merge;
use the new entry. TRACE BUILD MISMATCH means the OS/RBF/app did not match.
Do not switch instances without powering off. Capture is single-use per boot.

It performs the same bound-slot OPEN -> metadata -> FLUSH as B45, with ZERO
output writes and no retry. Startup boot MIF is unchanged. This is a tracing
package, NOT a confirmed FLUSH fix and NOT a ROM dump test.

After FLUSH/timeout a summary page appears. Send a photo of that whole page.
Then press RIGHT once and send the first event page (six events). LEFT/RIGHT
page through all events; A returns to summary. More pages can be requested
after those first observations. No file command is issued while paging.

Expected identity: TF TRACE smoke.46. SNAP RC 0 means the diagnostic snapshot
was read successfully, not that FLUSH succeeded. N is record count (max 256).
FLAGS low16=count, bit16=overflow, bit17=simultaneous event collision,
bit18=armed. RD/TX duplicate polling is filtered, so counts are NOT total bus
traffic. A full/collided snapshot is incomplete, not proof of missing traffic.
SLOT TX and SIZE TX show observed queued replies, not host receipt. SIZE TX
can be absent if host did not read the table, history filled, or pairing lacks
the relevant ID. Do not interpret NO as a zero-sized file.

Event words: [kind:4|ticks:28], address, raw wire-order data, current TX address.
AR=arm, RD=core saw host read, TX=word loaded into serial TX buffer paired with
previous read request, WR=core saw host write, ?T=TX with no previous read.
For TX, NEXT is the current request whose buffered reply returns older data.
RD has no data. Ticks are 74MHz bridge clocks, wrapping after 2^28 clocks;
not absolute time. Canonical F8 addresses ignore address bits23:16 for decode.
FPGA TX != HOST CONFIRM: this cannot prove what Pocket firmware received.

OS patch: exact smoke.44 image, fixed-size addition of one bounded read-only
SDK diagnostic syscall. Changes only offsets F084..F0BB and 16B3C..16B3F plus
internal CRC. File length 137480, startup/entry/BSS and FLUSH polling unchanged.
Old output-size and timeout-print patches retained, not newly proven correct.
App uses SDK calls only; no application MMIO. Trace freezes after the test,
copies into normal app RAM, then displays without host file operations.

Local verification: recorder 827 checks, AXI diagnostics 31, old handshake 65,
buffered command/table 154; snapshot mock 21 cases; RV32 service instructions
1113 checks; existing OPEN/FLUSH sequence mocks, SDK build, OS CRC and ZIP
payload hashes. These are bounded tests, not Pocket firmware/filesystem or a
full serial PHY simulation. Actual Pocket result remains PENDING.
Quartus minimum setup=$($slack.setup) ns, hold=$($slack.hold) ns.
"@
$provenance = @"
version=$Version
variant=smoke45_bound_flow_with_sdk_readonly_bridge_event_snapshot
base_os_sha256=$($critical[$osRel])
os_sha256=$(Get-Sha (Join-Path $sd $osRel))
os_internal_crc=$('{0:x8}' -f $crc.ComputedCrc)
os_length=137480
os_patch_regions=f084-f0bb,16b3c-16b3f,final_four_crc_bytes
os_startup_bss_poll_loops_unchanged=PASS
rbf_raw_sha256=$(Get-Sha $rawRbf)
rbf_r_sha256=$(Get-Sha (Join-Path $sd $rbfRel))
app_sha256=$(Get-Sha $elf)
boot_mif_sha256=$(Get-Sha (Join-Path $fpga 'firmware.mif'))
quartus_full_compile=PASS
quartus_mif_update=PASS
setup_slack_ns=$($slack.setup)
hold_slack_ns=$($slack.hold)
instance=TF Trace 46
initial_slot4_size=64
output_path=/Saves/chisburner/common/storage-smoke.41.bin
requested_size=1048613
output_write_calls=0
post_flush_file_commands=0
diagnostic_calls=SDK_bounded_read_only_syscall
hardware_result=PENDING
"@
[IO.File]::WriteAllText((Join-Path $bundle 'README_TEST.txt'),$readme,$utf8)
[IO.File]::WriteAllText((Join-Path $bundle 'BUILD_PROVENANCE.txt'),$provenance,$utf8)
Copy-Item -LiteralPath (Join-Path $fpga 'SOURCE_SHA256.txt') -Destination (Join-Path $bundle 'FPGA_SOURCE_SHA256.txt')
$softwareFiles = @('app\storage_event_probe\main.c','app\storage_event_probe\probe.h','app\storage_slot_probe\probe.h',
    '.integration\openfpgaos\src\firmware\api\of_bridge_trace.h','.integration\openfpgaos\src\firmware\api\of_syscall_numbers.h',
    'tests\patches\smoke44-bridge-trace-service.S','tests\storage-event-service-patch.ps1','tests\build-storage-events-smoke.ps1')
$softwareHashes = $softwareFiles | ForEach-Object { "$(Get-Sha (Join-Path $root $_))  $_" }
[IO.File]::WriteAllLines((Join-Path $bundle 'SOFTWARE_SOURCE_SHA256.txt'),[string[]]$softwareHashes,$utf8)
$hashes = @{}
$sums = Get-ChildItem -LiteralPath $sd -Recurse -File | Sort-Object FullName | ForEach-Object {
    $rel = $_.FullName.Substring($sdPrefix.Length).Replace('\','/')
    $hash = Get-Sha $_.FullName
    $hashes["SD_ROOT/$rel"] = $hash
    "$hash  $rel"
}
[IO.File]::WriteAllLines((Join-Path $bundle 'SHA256SUMS.txt'),[string[]]$sums,$utf8)
Compress-Archive -Path (Join-Path $bundle '*') -DestinationPath $zipPath -CompressionLevel Optimal
$null = Assert-ChisBurnerZipOsIntegrity -ZipPath $zipPath
$archive = [IO.Compression.ZipFile]::OpenRead($zipPath)
try {
    foreach ($name in $hashes.Keys) {
        $entry = $archive.GetEntry($name)
        if ($null -eq $entry) { throw "ZIP missing $name" }
        $stream = $entry.Open(); $sha = [Security.Cryptography.SHA256]::Create()
        try {
            $hash = [BitConverter]::ToString($sha.ComputeHash($stream)).Replace('-','').ToLowerInvariant()
            if ($hash -ne $hashes[$name]) { throw "ZIP mismatch: $name" }
        } finally { $stream.Dispose(); $sha.Dispose() }
    }
} finally { $archive.Dispose() }
Write-Output "ZIP CRC and payloads PASS: $zipPath"
Write-Output "ZIP SHA256: $(Get-Sha $zipPath)"
