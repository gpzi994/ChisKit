[CmdletBinding()]
param([string]$Distro='Ubuntu-24.04',[string]$OpenFpgaSdk='/home/lin13/work/ONS_FPGA/openfpgaSDK',
      [string]$Version='0.1.0-smoke.47-host-audit')
$ErrorActionPreference='Stop'
. (Join-Path $PSScriptRoot 'os-image-integrity.ps1')
function Get-Sha([string]$Path) { (Get-FileHash -LiteralPath $Path).Hash.ToLowerInvariant() }
function Wsl-Path([string]$Path) {
    $full=[IO.Path]::GetFullPath($Path)
    if($full -notmatch '^([A-Za-z]):\\(.*)$') { throw 'Expected local drive path' }
    '/mnt/'+$Matches[1].ToLowerInvariant()+'/'+$Matches[2].Replace('\','/')
}
if($Version -notmatch '^0\.1\.0-smoke\.47-[a-z0-9-]+$' -or $Version.Length -gt 31) { throw 'Invalid version' }
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$base=Join-Path $root 'build\smoke\ChisBurner-storage-flush-0.1.0-smoke.46-host-events\SD_ROOT'
$fpga=Join-Path $root '.integration\openfpgaos\src\fpga\targets\pocket\bld\chisburner-storage-audit-s47c-win'
$bundle=Join-Path $root "build\smoke\ChisBurner-storage-flush-$Version"
$zipPath="$bundle.zip"
if((Test-Path -LiteralPath $bundle) -or (Test-Path -LiteralPath $zipPath)) { throw 'Preserve existing package' }
$osRel='Assets\chisburner\common\os.bin'
$appRel='Assets\chisburner\common\chisburnersmoke.elf'
$rbfRel='Cores\ChisBurner.ChisBurner\chis.rbf_r'
$coreRel='Cores\ChisBurner.ChisBurner\core.json'
$oldInstance='Assets\chisburner\ChisBurner.ChisBurner\TF Trace 46.json'
$newInstance='Assets\chisburner\ChisBurner.ChisBurner\TF Audit 47.json'
$critical=@{
    $osRel='17c8cae47bd24869e5fbaa01514ffcd0f5ffc77fd514f5305bd3eeb1a719f7fa'
    $rbfRel='dd9f6fcb8fcbd1433ce6f7f37b34262b4f0e6f91d925d40de104f61253167ebe'
    $appRel='b4e4846e22505bbdf5e604f1bb2b15279eca3bd38dabf45c5d11ee6e6cfc3ff0'
}
foreach($rel in $critical.Keys) {
    if((Get-Sha (Join-Path $base $rel)) -ne $critical[$rel]) { throw "Base mismatch: $rel" }
}
$crc=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes((Join-Path $base $osRel)))
$qsf=Get-Content -Raw -LiteralPath (Join-Path $fpga 'ap_core.qsf')
foreach($macro in @('CHISBURNER_STORAGE_TRACE','CHISBURNER_STORAGE_EVENTS','CHISBURNER_STORAGE_AUDIT')) {
    if($qsf -notmatch "(?m)^set_global_assignment -name VERILOG_MACRO $macro\s*$") { throw "Missing $macro" }
}
if($qsf -match '(?m)^set_global_assignment -name VERILOG_MACRO INCLUDE_CARTRIDGE\s*$') { throw 'Wrong profile' }
foreach($stage in @('map','fit','asm','mif_update')) {
    $rpt=Get-Content -Raw -LiteralPath (Join-Path $fpga "output_files\ap_core.$stage.rpt")
    if($rpt -notmatch 'was successful\. 0 errors') { throw "FPGA $stage failed" }
}
$map=Get-Content -Raw -LiteralPath (Join-Path $fpga 'output_files\ap_core.map.rpt')
if($map -notmatch 'pct_bridge_audit:storage_events\|altsyncram:events_rtl_0.*;\s*M10K block\s*;\s*Simple Dual Port\s*;') { throw 'Audit event RAM was not inferred as dual-port M10K' }
if($map -notmatch 'pct_bridge_audit:storage_events\|altsyncram:last_tx_rtl_0.*;\s*M10K block\s*;\s*Simple Dual Port\s*;') { throw 'Audit history RAM was not inferred as dual-port M10K' }
$mif=Get-Content -Raw -LiteralPath (Join-Path $fpga 'output_files\ap_core.mif_update.rpt')
if($mif -notmatch 'Processed Memory Initialization File .*firmware\.mif' -or
   $mif -notmatch 'Processed Memory Initialization File .*build_id\.mif' -or
   $mif -match 'Critical Warning|can.t find|setting all initial values to 0') { throw 'Boot embedding unverified' }
if((Get-Sha (Join-Path $fpga 'firmware.mif')) -ne 'aade051a2a517f71d55f1610a3b4e6379e73896c68c322a96556f3982befbba2') { throw 'Boot firmware changed' }
foreach($line in Get-Content -LiteralPath (Join-Path $fpga 'SOURCE_SHA256.txt')) {
    if($line -notmatch '^([0-9a-f]{64})  (.+)$') { throw 'Malformed source manifest' }
    $hash=$Matches[1]; $path=$Matches[2]
    if((Get-Sha $path) -ne $hash) { throw "FPGA source changed during compile: $path" }
}
$sta=Get-Content -Raw -LiteralPath (Join-Path $fpga 'output_files\ap_core.sta.rpt')
$slack=@{}
foreach($kind in @('setup','hold')) {
    $m=[regex]::Matches($sta,"Worst-case $kind slack is\s+(-?\d+(?:\.\d+)?)")
    if(!$m.Count) { throw "Missing $kind slack" }
    $slack[$kind]=($m | ForEach-Object { [double]$_.Groups[1].Value } | Measure-Object -Minimum).Minimum
    if($slack[$kind] -le 0) { throw "Timing failed: $kind" }
}
$checks=@{
    'storage-audit\test.log'='76 PASS, 0 FAIL'
    'storage-audit-periph\test.log'='34 passed, 0 failed'
    'storage-tx-tap\test.log'='896 PASS, 0 FAIL'
    'storage-buffered\test.log'='154 PASS, 0 FAIL'
    'storage-trace\bridge-test.log'='50 passed, 0 failed'
    'storage-trace\periph-test.log'='15 passed, 0 failed'
}
foreach($rel in $checks.Keys) {
    if(!(Get-Content -Raw -LiteralPath (Join-Path $root "build\rtl\$rel")).Contains($checks[$rel])) { throw "Test failed: $rel" }
}
$appRoot=Join-Path $root 'app\storage_audit_probe'
if((Get-Sha (Join-Path $root 'app\storage_slot_probe\probe.h')) -ne 'eb44f7a5a430b6e5709233af4c1606d6a202ecffcdda61b1ebce269535ae4048') { throw 'OPEN/FLUSH sequence changed since smoke46' }
if((Get-Sha (Join-Path $root '.integration\openfpgaos\src\firmware\api\of_bridge_trace.h')) -ne 'b679ba0fd00fc6ea904ad1e583a693e17b840f96b94c6971b321e41f99ca4bbc') { throw 'Underlying SDK diagnostic ABI changed' }
$appWsl=Wsl-Path $appRoot
$source=(Get-Content -Raw -LiteralPath (Join-Path $appRoot 'main.c'))+(Get-Content -Raw -LiteralPath (Join-Path $appRoot 'probe.h'))
if($source -match 'of_cart_|0x40000|of_output_write\s*\(') { throw 'Application must remain SDK-only with no writes' }
& wsl.exe -d $Distro -- bash -lc "cd '$appWsl' && make clean OPENFPGA_SDK='$OpenFpgaSdk' && make host-test all OPENFPGA_SDK='$OpenFpgaSdk' && cd ../storage_slot_probe && make host-test OPENFPGA_SDK='$OpenFpgaSdk'"
if($LASTEXITCODE -ne 0) { throw 'Application SDK build/test failed' }
$elf=Join-Path $root 'build\storage-audit-app\app.elf'
if(![Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($elf)).Contains('TF AUDIT smoke.47')) { throw 'Wrong app identity' }
New-Item -ItemType Directory -Path $bundle | Out-Null
Copy-Item -LiteralPath $base -Destination $bundle -Recurse
$sd=Join-Path $bundle 'SD_ROOT'
$sdPrefix=[IO.Path]::GetFullPath($sd).TrimEnd('\')+'\'
Copy-Item -LiteralPath $elf -Destination (Join-Path $sd $appRel)
$from=[IO.Path]::GetFullPath((Join-Path $sd $oldInstance))
$to=[IO.Path]::GetFullPath((Join-Path $sd $newInstance))
if(!$from.StartsWith($sdPrefix) -or !$to.StartsWith($sdPrefix)) { throw 'Unsafe staging move' }
Move-Item -LiteralPath $from -Destination $to
$utf8=New-Object Text.UTF8Encoding($false)
$corePath=Join-Path $sd $coreRel
$core=Get-Content -Raw -LiteralPath $corePath | ConvertFrom-Json
if([int64]$core.core.framework.hardware.cartridge_adapter -ne -1) { throw 'Cartridge must remain disabled' }
$core.core.metadata.version=$Version
$core.core.metadata.date_release='2026-09-03'
[IO.File]::WriteAllText($corePath,($core | ConvertTo-Json -Depth 20),$utf8)
if(-not ('ChisBurner.Build.ReverseRbf' -as [type])) {
    Add-Type -TypeDefinition @'
namespace ChisBurner.Build {
    public static class ReverseRbf {
        public static byte[] Apply(byte[] input) {
            var t=new byte[256];
            for(int i=0;i<256;i++){int x=i,y=0;for(int b=0;b<8;b++){y=(y<<1)|(x&1);x>>=1;}t[i]=(byte)y;}
            var output=new byte[input.Length];for(int i=0;i<input.Length;i++)output[i]=t[input[i]];return output;
        }
    }
}
'@
}
$rawRbf=Join-Path $fpga 'output_files\ap_core.rbf'
[IO.File]::WriteAllBytes((Join-Path $sd $rbfRel),[ChisBurner.Build.ReverseRbf]::Apply([IO.File]::ReadAllBytes($rawRbf)))
if([BitConverter]::ToString([ChisBurner.Build.ReverseRbf]::Apply([byte[]](1,128,18,170))) -ne '80-01-48-55') { throw 'Bit reversal failed' }
$sha=[Security.Cryptography.SHA256]::Create()
try { $hash=[BitConverter]::ToString($sha.ComputeHash([ChisBurner.Build.ReverseRbf]::Apply([IO.File]::ReadAllBytes((Join-Path $sd $rbfRel))))).Replace('-','').ToLowerInvariant() }
finally { $sha.Dispose() }
if($hash -ne (Get-Sha $rawRbf)) { throw 'RBF round-trip failed' }
$basePrefix=[IO.Path]::GetFullPath($base).TrimEnd('\')+'\'
foreach($file in Get-ChildItem -LiteralPath $base -Recurse -File) {
    $rel=$file.FullName.Substring($basePrefix.Length)
    if($rel -in @($appRel,$rbfRel,$coreRel)) { continue }
    $dest=if($rel -eq $oldInstance){$newInstance}else{$rel}
    if((Get-Sha $file.FullName) -ne (Get-Sha (Join-Path $sd $dest))) { throw "Unexpected payload change: $rel" }
}
$readme=@"
ChisBurner - smoke.47 OPEN / FLUSH communication audit

Keep the cartridge slot EMPTY. Merge ALL SD_ROOT, including Saves, and approve
replacement. Only the old storage-smoke.41.bin diagnostic placeholder is reset;
no cartridge save is used. Power OFF/ON; choose "TF Audit 47.json" in the core.
Its path is Assets/chisburner/ChisBurner.ChisBurner/TF Audit 47.json.
Screen MUST say "TF AUDIT smoke.47". A different number means stale application.
AUDIT BUILD/ARM FAIL means mismatched payloads or diagnostic arm failure;
in that case the app issues NO file commands. Old launch entries may remain.

Run once. After OPEN/FLUSH returns (including timeout), send the first THREE
screen pages: summary, OPEN WINDOW, FLUSH WINDOW. RIGHT advances; A returns
to summary. Further pages contain changed-field events, only if needed.
Do not switch instances without a cold power cycle: recorder is single-use.

The OS is byte-for-byte smoke46 (137480 bytes), including its existing SDK
diagnostic service and prior fixes. No OS relink or additional binary patch.
Same boot MIF, same data.json, same bound 64-byte seed, same diagnostic path
and requested size 1048613. Same OPEN -> metadata -> FLUSH sequence, no output
WRITE, no retry. Diagnostic arming/ack happens before the file sequence; the
recorder is frozen only after the entire sequence, never between OPEN/FLUSH.
This is diagnostic, NOT a confirmed FLUSH fix and NOT a ROM dump package.

Recording windows start on FPGA OPEN and FLUSH request edges. OPEN window also
includes the short post-OPEN application metadata/printing gap. Counters cover
the full respective window until FLUSH starts / snapshot freezes. They do not
mean the host accepted the command. FPGA queued data and serializer completion
cannot prove that Pocket firmware received or interpreted the bits correctly.

FLAGS bit16=log overflow, bit17=event collision (including phase-boundary
collision), bit18=armed, bit19=32-bit timestamp saturation. Low16=log count.
Log overflow/collision loses detail, but counters continue. Ordinary counters
saturate at FFFFFFFF: interpret that value as a lower bound, never exact.
LOST has a conservative saturation guard: LOST=FFFFFFFF means at least
FFFFFFF9 lost events, not necessarily FFFFFFFF. OUTSIDE the
phase windows no traffic is attributed to OPEN or FLUSH.

CM/BU/OK are COUNTS: matching command queued for TX / busy status writes seen /
completion status writes seen. They are not boolean PASS indicators.
F8 R/T/W counts every F8 read / queued reply / write, before log filtering.
ALL TX Q/D/A counts all queued / finished / aborted serial transfers, including
non-F8 payload accesses. A transfer crossing a phase/freeze boundary can leave
a small Q/D difference; DONE does not mean host receipt. Abort means SS ended
before the local serializer reached its normal completion state.
TABLE RD is whole-window slot-table read count, not just saved log entries.
SLOT and SIZE require the corresponding YES flag; NO does not mean value zero.
CMD first -> last and LAST RD/TX show whole-window observations even if log full.
OTHER R/T counts F8 addresses outside mapped dedup fields / unpaired replies.
LOST counts events excluded by collision/full (phase markers not included).

Log: first read of each canonical field, then only changed TX values, every
host write, and serial aborts. Dedup resets between OPEN and FLUSH. Target/host
registers (32 words each) and all 64 slot-table words have independent keys.
Other addresses are unfiltered. P=O/F identifies the window; PH marks its start.
T64 is timestamp divided by 64, about 0.862 microseconds per unit. Phase detail
TICKS and header time are raw 74.25MHz clocks. Raw time saturates after ~57.8s.
The four event words remain kind/phase/time, address, wire data, current request.

The observation-only input pipeline delays all traffic and phase edges by one
bridge clock together. Freeze drains it. Per-field TX history uses synchronous
RAM with explicit same-address read/write forwarding; command hardware is not
stalled or changed by this pipeline.

Local checks: 76 audit assertions including 4096 multi-address polling rounds,
overflow counters, both phases and adjacent history collisions; 34 AXI diagnostics; 65 legacy handshake;
154 buffered table/command; 100 SDK mocks and unchanged file-sequence mocks.
896 serializer-state tap checks cover 32 clock phases, both endian modes, and
normal/aborted transactions. That simulation uses a TEST-ONLY lowering of the
original procedural tristate drivers to explicit OE/data drivers for Verilator;
production RTL is untouched by that adapter. It does not validate electrical
waveforms, bus contention on hardware, or Pocket's receipt of the outgoing word.
These bounded tests are NOT Pocket firmware/filesystem or full electrical PHY
simulation. Actual hardware outcome remains PENDING.
Quartus constrained setup minimum=$($slack.setup) ns; hold minimum=$($slack.hold) ns.
Inherited platform constraints still report not fully constrained; positive
slack here is not proof of every external pin, clock crossing or electrical PHY.
"@
$provenance=@"
version=$Version
base=smoke46
os_byte_identical_smoke46=PASS
os_sha256=$(Get-Sha (Join-Path $sd $osRel))
os_internal_crc=$('{0:x8}' -f $crc.ComputedCrc)
os_length=$($crc.Length)
rbf_raw_sha256=$(Get-Sha $rawRbf)
rbf_r_sha256=$(Get-Sha (Join-Path $sd $rbfRel))
app_sha256=$(Get-Sha $elf)
boot_mif_sha256=$(Get-Sha (Join-Path $fpga 'firmware.mif'))
setup_slack_ns=$($slack.setup)
hold_slack_ns=$($slack.hold)
event_ram_inferred_M10K=PASS
history_ram_inferred_M10K=PASS
serializer_state_tap_checks=896_PASS_with_test_only_pad_adapter
file_sequence=OPEN_metadata_FLUSH_unchanged
output_write_calls=0
post_flush_file_commands=0
between_commands_snapshot_or_delay=NONE
instance=TF Audit 47
hardware_result=PENDING
"@
[IO.File]::WriteAllText((Join-Path $bundle 'README_TEST.txt'),$readme,$utf8)
[IO.File]::WriteAllText((Join-Path $bundle 'BUILD_PROVENANCE.txt'),$provenance,$utf8)
Copy-Item -LiteralPath (Join-Path $fpga 'SOURCE_SHA256.txt') -Destination (Join-Path $bundle 'FPGA_SOURCE_SHA256.txt')
$software=@('app\storage_audit_probe\main.c','app\storage_audit_probe\probe.h','app\storage_audit_probe\test_probe.c',
    'app\storage_slot_probe\probe.h','.integration\openfpgaos\src\firmware\api\of_bridge_audit.h',
    '.integration\openfpgaos\src\firmware\api\of_bridge_trace.h','tests\build-storage-audit-smoke.ps1')
$sums=$software | ForEach-Object { "$(Get-Sha (Join-Path $root $_))  $_" }
[IO.File]::WriteAllLines((Join-Path $bundle 'SOFTWARE_SOURCE_SHA256.txt'),[string[]]$sums,$utf8)
$hashes=@{}
$sums=Get-ChildItem -LiteralPath $sd -Recurse -File | Sort-Object FullName | ForEach-Object {
    $rel=$_.FullName.Substring($sdPrefix.Length).Replace('\','/');$h=Get-Sha $_.FullName
    $hashes["SD_ROOT/$rel"]=$h;"$h  $rel"
}
[IO.File]::WriteAllLines((Join-Path $bundle 'SHA256SUMS.txt'),[string[]]$sums,$utf8)
Compress-Archive -Path (Join-Path $bundle '*') -DestinationPath $zipPath -CompressionLevel Optimal
$null=Assert-ChisBurnerZipOsIntegrity -ZipPath $zipPath
$archive=[IO.Compression.ZipFile]::OpenRead($zipPath)
try {
    foreach($name in $hashes.Keys) {
        $entry=$archive.GetEntry($name);if($null -eq $entry){throw "ZIP missing $name"}
        $stream=$entry.Open();$sha=[Security.Cryptography.SHA256]::Create()
        try {
            $h=[BitConverter]::ToString($sha.ComputeHash($stream)).Replace('-','').ToLowerInvariant()
            if($h -ne $hashes[$name]){throw "ZIP mismatch: $name"}
        } finally {$stream.Dispose();$sha.Dispose()}
    }
} finally {$archive.Dispose()}
"ZIP CRC and payloads PASS: $zipPath"
"ZIP SHA256: $(Get-Sha $zipPath)"
