[CmdletBinding()]
param([string]$Distro = 'Ubuntu-24.04',
      [string]$OpenFpgaSdk = '/home/lin13/work/ONS_FPGA/openfpgaSDK',
      [string]$Version = '0.1.0-smoke.44-host-trace')
$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'storage-timeout-trace-patch.ps1')
function Get-Sha([string]$Path) {
    (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant()
}
function Wsl-Path([string]$Path) {
    $full = [IO.Path]::GetFullPath($Path)
    if ($full -notmatch '^([A-Za-z]):\\(.*)$') { throw 'Expected local drive path' }
    '/mnt/' + $Matches[1].ToLowerInvariant() + '/' + $Matches[2].Replace('\','/')
}
if ($Version -notmatch '^0\.1\.0-smoke\.44-[a-z0-9-]+$' -or $Version.Length -gt 31) {
    throw 'Invalid smoke.44 version'
}
$root = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$baseSd = Join-Path $root 'build\smoke\ChisBurner-storage-flush-0.1.0-smoke.43-no-write\SD_ROOT'
$fpga = Join-Path $root '.integration\openfpgaos\src\fpga\targets\pocket\bld\chisburner-storage-trace-s44-win'
$bundle = Join-Path $root "build\smoke\ChisBurner-storage-flush-$Version"
$zipPath = "$bundle.zip"
if ((Test-Path -LiteralPath $bundle) -or (Test-Path -LiteralPath $zipPath)) { throw 'Preserve existing package' }
$osRel = 'Assets\chisburner\common\os.bin'
$appRel = 'Assets\chisburner\common\chisburnersmoke.elf'
$rbfRel = 'Cores\ChisBurner.ChisBurner\chis.rbf_r'
$coreRel = 'Cores\ChisBurner.ChisBurner\core.json'
$oldInstance = 'Assets\chisburner\ChisBurner.ChisBurner\TF Flush NOWRITE 43.json'
$newInstance = 'Assets\chisburner\ChisBurner.ChisBurner\TF Flush TRACE 44.json'
if ((Get-Sha (Join-Path $baseSd $osRel)) -ne 'b627365bef8b297e5362bd00863c57cfc0507459aebf5d5379f1cd23c26d24e8' -or
    (Get-Sha (Join-Path $baseSd $rbfRel)) -ne 'b3c3d8b013859b555dce81cf556c342b0ab8ccf460418d9b4b32f1642784dd4b' -or
    (Get-Sha (Join-Path $baseSd $appRel)) -ne '2700b06dc37796bc281599bef04fd646806e73c66b920e097487abcc3a2384a1') {
    throw 'Smoke.43 base payload mismatch'
}
$qsf = Get-Content -Raw -LiteralPath (Join-Path $fpga 'ap_core.qsf')
if ($qsf -notmatch '(?m)^set_global_assignment -name VERILOG_MACRO CHISBURNER_STORAGE_TRACE\s*$' -or
    $qsf -match '(?m)^set_global_assignment -name VERILOG_MACRO INCLUDE_CARTRIDGE\s*$') { throw 'Wrong trace profile' }
foreach ($stage in @('map','fit','asm','mif_update')) {
    $rpt = Get-Content -Raw -LiteralPath (Join-Path $fpga "output_files\ap_core.$stage.rpt")
    if ($rpt -notmatch 'was successful\. 0 errors') { throw "FPGA $stage did not pass" }
}
$mifReport = Get-Content -Raw -LiteralPath (Join-Path $fpga 'output_files\ap_core.mif_update.rpt')
if ($mifReport -notmatch 'Processed Memory Initialization File .*firmware\.mif' -or
    $mifReport -notmatch 'Processed Memory Initialization File .*build_id\.mif' -or
    $mifReport -match 'Critical Warning|can.t find|setting all initial values to 0') { throw 'Boot MIF embedding unverified' }
if ((Get-Sha (Join-Path $fpga 'firmware.mif')) -ne 'aade051a2a517f71d55f1610a3b4e6379e73896c68c322a96556f3982befbba2') {
    throw 'Wrong embedded boot firmware input'
}
foreach ($line in Get-Content -LiteralPath (Join-Path $fpga 'SOURCE_SHA256.txt')) {
    if ($line -notmatch '^([0-9a-f]{64})  (.+)$') { throw 'Malformed source manifest' }
    $expected = $Matches[1]; $path = $Matches[2]
    if ((Get-Sha $path) -ne $expected) { throw "Source changed since compile: $path" }
}
$sta = Get-Content -Raw -LiteralPath (Join-Path $fpga 'output_files\ap_core.sta.rpt')
$slack = @{}
foreach ($kind in @('setup','hold')) {
    $m = [regex]::Matches($sta, "Worst-case $kind slack is\s+(-?\d+(?:\.\d+)?)")
    if ($m.Count -eq 0) { throw "Missing $kind slack" }
    $slack[$kind] = ($m | ForEach-Object { [double]$_.Groups[1].Value } | Measure-Object -Minimum).Minimum
    if ($slack[$kind] -le 0) { throw "Timing failed: $kind" }
}
foreach ($pair in @(@('bridge','50'),@('periph','15'))) {
    $log = Get-Content -Raw -LiteralPath (Join-Path $root "build\rtl\storage-trace\$($pair[0])-test.log")
    if (-not $log.Contains("$($pair[1]) passed, 0 failed")) { throw 'Trace RTL tests not passed' }
}
$appDir = Wsl-Path (Join-Path $root 'app\storage_flush_probe')
if ((Get-Sha (Join-Path $root 'app\storage_flush_probe\probe.h')) -ne '0ec29c4f3cdfded5b13ca4483378e3ed718f81042469d3c4e504cd5d54048a40') {
    throw 'OPEN -> FLUSH sequence changed relative to smoke.43'
}
$appBuild = Wsl-Path (Join-Path $root 'build\storage-trace-app')
$patchBuild = Wsl-Path (Join-Path $root 'build\storage-trace-patch')
$patchSource = Wsl-Path (Join-Path $root 'tests\patches\smoke42-timeout-trace.S')
& wsl.exe -d $Distro -- bash -lc "cd '$appDir' && make clean OPENFPGA_SDK='$OpenFpgaSdk' BUILD_DIR='$appBuild' && make host-test all OPENFPGA_SDK='$OpenFpgaSdk' BUILD_DIR='$appBuild' TRACE_SMOKE44=1 && mkdir -p '$patchBuild' && riscv64-unknown-elf-as -march=rv32imafc -mabi=ilp32f -o '$patchBuild/patch.o' '$patchSource' && riscv64-unknown-elf-objcopy -O binary -j .text '$patchBuild/patch.o' '$patchBuild/patch.bin'"
if ($LASTEXITCODE -ne 0) { throw 'SDK application/patch build failed' }
$elfText = [Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes((Join-Path $root 'build\storage-trace-app\app.elf')))
if (-not $elfText.Contains('TF FLUSH smoke.44') -or $elfText.Contains('TF FLUSH smoke.43')) {
    throw 'The compiled ELF has the wrong on-screen identity'
}
& (Join-Path $PSScriptRoot 'test-storage-trace-patch.ps1')
$osBytes = Set-ChisBurnerTimeoutTracePatch -Bytes ([IO.File]::ReadAllBytes((Join-Path $baseSd $osRel))) `
    -Instructions ([IO.File]::ReadAllBytes((Join-Path $root 'build\storage-trace-patch\patch.bin')))
$crc = Assert-ChisBurnerOsIntegrity -Bytes $osBytes

New-Item -ItemType Directory -Path $bundle | Out-Null
Copy-Item -LiteralPath $baseSd -Destination $bundle -Recurse
$sd = Join-Path $bundle 'SD_ROOT'
[IO.File]::WriteAllBytes((Join-Path $sd $osRel), $osBytes)
Copy-Item -LiteralPath (Join-Path $root 'build\storage-trace-app\app.elf') -Destination (Join-Path $sd $appRel) -Force
Move-Item -LiteralPath (Join-Path $sd $oldInstance) -Destination (Join-Path $sd $newInstance)
$utf8 = New-Object Text.UTF8Encoding($false)
$corePath = Join-Path $sd $coreRel
$core = Get-Content -Raw -LiteralPath $corePath | ConvertFrom-Json
if ([int64]$core.core.framework.hardware.cartridge_adapter -ne -1) { throw 'Cartridge must remain disabled' }
$core.core.metadata.version = $Version
$core.core.metadata.date_release = '2026-09-03'
[IO.File]::WriteAllText($corePath, ($core | ConvertTo-Json -Depth 20), $utf8)
if (-not ('ChisBurner.Build.ReverseRbf' -as [type])) {
    Add-Type -TypeDefinition @'
namespace ChisBurner.Build {
    public static class ReverseRbf {
        public static byte[] Apply(byte[] input) {
            var table = new byte[256];
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
[IO.File]::WriteAllBytes((Join-Path $sd $rbfRel), [ChisBurner.Build.ReverseRbf]::Apply($raw))
$reversedVector = [ChisBurner.Build.ReverseRbf]::Apply([byte[]](0x01,0x80,0x12,0xAA))
if ([BitConverter]::ToString($reversedVector) -ne '80-01-48-55') { throw 'RBF bit-reversal vector failed' }
$roundtrip = [ChisBurner.Build.ReverseRbf]::Apply([IO.File]::ReadAllBytes((Join-Path $sd $rbfRel)))
$sha = [Security.Cryptography.SHA256]::Create()
try { $roundtripHash = [BitConverter]::ToString($sha.ComputeHash($roundtrip)).Replace('-','').ToLowerInvariant() }
finally { $sha.Dispose() }
if ($roundtripHash -ne (Get-Sha $rawRbf)) { throw 'RBF round-trip mismatch' }
$basePrefix = [IO.Path]::GetFullPath($baseSd).TrimEnd('\') + '\'
foreach ($f in Get-ChildItem -LiteralPath $baseSd -Recurse -File) {
    $rel = $f.FullName.Substring($basePrefix.Length)
    if ($rel -in @($osRel,$appRel,$rbfRel,$coreRel)) { continue }
    $dest = if ($rel -eq $oldInstance) { $newInstance } else { $rel }
    if ((Get-Sha $f.FullName) -ne (Get-Sha (Join-Path $sd $dest))) { throw "Unexpected payload change: $rel" }
}
$readme = @"
ChisBurner - smoke.44 host handshake trace

Keep the cartridge slot EMPTY. Merge ALL of SD_ROOT, including Saves, and
approve replacement. The seed replaces only the old storage-smoke.41.bin
diagnostic file. Power OFF/ON. Select "TF Flush TRACE 44".
Screen must say "TF FLUSH smoke.44"; an older number means stale application.
Old launch entries can remain after merging; use the new entry above.

The application still performs OPEN -> FLUSH with ZERO output WRITE commands.
No retries. No cartridge access. This is NOT confirmation of a FLUSH fix.
Report OPEN, FLUSH RC and the complete timeout line, especially st=XXXXXXXX.
An ACK/DONE timeout should now have eight hex digits starting with D; if it
still shows only st=20, the OS patch was not loaded. All zeros can indicate a
stale RBF without the trace register. Report startup errors separately.

New FPGA: adds observation-only trace, with the proven smoke.31 boot MIF.
OS: exact smoke.42 base with four fixed-width error-print instructions changed;
startup layout and command polling unchanged; internal OFC1 CRC regenerated.
The earlier output-size patch is retained, not newly validated.
Normal DS_STATUS is unchanged; the timeout print reads dedicated DS_TRACE.

st format: [31:28]=D; [27:12]=bridge trace; [11]=CPU active, [10]=CPU flush
request, [9]=CPU synced ACK, [8]=CPU synced DONE; [7:0]=normal DS_STATUS.
Bridge trace: [15:12]=FSM state, [11:9]=prefix (1 cm, 2 bu, 3 ok, 0 other),
[8]=low target command still 0188; sticky bits [7:0]=request, launched,
host read command, host read slot, host wrote status, host wrote busy,
host wrote complete, queue observed. Debug CDC is read only after timeout;
it is not an atomic live-control snapshot. Flags retain the most recent FLUSH.

Local checks: 65 RTL assertions, 6 application sequence mock cases, exact-base
and patch-range guards, OS CRC and all ZIP payload hashes. Hardware pending.
Quartus minimum setup=$($slack.setup) ns, hold=$($slack.hold) ns.
"@
$provenance = @"
version=$Version
variant=smoke43_no_write_with_cold_handshake_trace
base_os_sha256=b627365bef8b297e5362bd00863c57cfc0507459aebf5d5379f1cd23c26d24e8
os_sha256=$(Get-Sha (Join-Path $sd $osRel))
os_internal_crc=$('{0:x8}' -f $crc.ComputedCrc)
os_patch_regions=933c-933f,934c-934f,9356-9359,9366-9369,final_four_crc_bytes
os_startup_and_poll_loops_unchanged=PASS
rbf_raw_sha256=$(Get-Sha $rawRbf)
rbf_r_sha256=$(Get-Sha (Join-Path $sd $rbfRel))
app_sha256=$(Get-Sha (Join-Path $sd $appRel))
boot_mif_sha256=$(Get-Sha (Join-Path $fpga 'firmware.mif'))
quartus_full_compile=PASS
quartus_mif_update=PASS
setup_slack_ns=$($slack.setup)
hold_slack_ns=$($slack.hold)
trace_rtl_checks=65_PASS
hardware_result=PENDING
"@
[IO.File]::WriteAllText((Join-Path $bundle 'README_TEST.txt'), $readme, $utf8)
[IO.File]::WriteAllText((Join-Path $bundle 'BUILD_PROVENANCE.txt'), $provenance, $utf8)
Copy-Item -LiteralPath (Join-Path $fpga 'SOURCE_SHA256.txt') -Destination $bundle
$prefix = [IO.Path]::GetFullPath($sd).TrimEnd('\') + '\'
$hashes = @{}
$sums = Get-ChildItem -LiteralPath $sd -Recurse -File | Sort-Object FullName | ForEach-Object {
    $rel = $_.FullName.Substring($prefix.Length).Replace('\','/')
    $hash = Get-Sha $_.FullName
    $hashes["SD_ROOT/$rel"] = $hash
    "$hash  $rel"
}
[IO.File]::WriteAllLines((Join-Path $bundle 'SHA256SUMS.txt'), [string[]]$sums, $utf8)
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
