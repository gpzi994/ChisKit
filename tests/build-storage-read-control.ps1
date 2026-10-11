[CmdletBinding()]
param([string]$Distro='Ubuntu-24.04',
      [string]$OpenFpgaSdk='/home/lin13/work/ONS_FPGA/openfpgaSDK',
      [string]$Version='0.1.0-smoke.48-read4-control')
$ErrorActionPreference='Stop'
. (Join-Path $PSScriptRoot 'os-image-integrity.ps1')
function Get-Sha([string]$Path){(Get-FileHash -LiteralPath $Path).Hash.ToLowerInvariant()}
if($Version -notmatch '^0\.1\.0-smoke\.48-[a-z0-9-]+$' -or $Version.Length -gt 31){throw 'Invalid version'}
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$baseBundle=Join-Path $root 'build\smoke\ChisBurner-storage-flush-0.1.0-smoke.47-host-audit'
$base=Join-Path $baseBundle 'SD_ROOT'
$bundle=Join-Path $root "build\smoke\ChisBurner-storage-flush-$Version"
$zipPath="$bundle.zip"
if((Test-Path -LiteralPath $bundle) -or (Test-Path -LiteralPath $zipPath)){throw 'Preserve existing package'}
$osRel='Assets\chisburner\common\os.bin'
$appRel='Assets\chisburner\common\chisburnersmoke.elf'
$rbfRel='Cores\ChisBurner.ChisBurner\chis.rbf_r'
$coreRel='Cores\ChisBurner.ChisBurner\core.json'
$oldInstance='Assets\chisburner\ChisBurner.ChisBurner\TF Audit 47.json'
$newInstance='Assets\chisburner\ChisBurner.ChisBurner\TF Read4 48.json'
$critical=@{
    $osRel='17c8cae47bd24869e5fbaa01514ffcd0f5ffc77fd514f5305bd3eeb1a719f7fa'
    $rbfRel='b0c016f3c8956263aef31d8a934b366413a45885d0f7e72080bd605ae2197223'
    $appRel='df35b36aca5a1e4134ac21eed34c7c7d576e55a7f2f51019d7578bca4d67c969'
}
foreach($rel in $critical.Keys){
    if((Get-Sha (Join-Path $base $rel)) -ne $critical[$rel]){throw "Base mismatch: $rel"}
}
foreach($line in Get-Content -LiteralPath (Join-Path $baseBundle 'SHA256SUMS.txt')){
    if($line -notmatch '^([0-9a-f]{64})  (.+)$'){throw 'Bad base manifest'}
    $expected=$Matches[1];$rel=$Matches[2]
    if((Get-Sha (Join-Path $base $rel)) -ne $expected){throw "Base file changed: $rel"}
}
$crc=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes((Join-Path $base $osRel)))
$appRoot=Join-Path $root 'app\storage_read_control'
$appFull=[IO.Path]::GetFullPath($appRoot)
if($appFull -notmatch '^([A-Za-z]):\\(.*)$'){throw 'Expected local drive path'}
$appWsl='/mnt/'+$Matches[1].ToLowerInvariant()+'/'+$Matches[2].Replace('\','/')
$source=(Get-Content -Raw -LiteralPath (Join-Path $appRoot 'main.c'))+
        (Get-Content -Raw -LiteralPath (Join-Path $appRoot 'probe.h'))+
        (Get-Content -Raw -LiteralPath (Join-Path $appRoot 'read4.h'))
if($source -match 'of_cart_|0x40000|of_output_write\s*\(|\bfopen\s*\(|\bread\s*\('){throw 'Must use SDK direct read with no MMIO, cached read or writes'}
& wsl.exe -d $Distro -- bash -lc "cd '$appWsl' && diff --strip-trailing-cr '$OpenFpgaSdk/src/sdk/include/of_file.h' ../../.integration/openfpgaos/src/firmware/api/of_file.h && make clean OPENFPGA_SDK='$OpenFpgaSdk' && make host-test all OPENFPGA_SDK='$OpenFpgaSdk'"
if($LASTEXITCODE -ne 0){throw 'SDK application build/tests failed'}
$elf=Join-Path $root 'build\storage-read-control-app\app.elf'
if(![Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($elf)).Contains('TF READ4 smoke.48')){throw 'Wrong app identity'}
New-Item -ItemType Directory -Path $bundle | Out-Null
Copy-Item -LiteralPath $base -Destination $bundle -Recurse
$sd=Join-Path $bundle 'SD_ROOT'
$sdPrefix=[IO.Path]::GetFullPath($sd).TrimEnd('\')+'\'
Copy-Item -LiteralPath $elf -Destination (Join-Path $sd $appRel)
$from=[IO.Path]::GetFullPath((Join-Path $sd $oldInstance))
$to=[IO.Path]::GetFullPath((Join-Path $sd $newInstance))
if(!$from.StartsWith($sdPrefix) -or !$to.StartsWith($sdPrefix)){throw 'Unsafe staging move'}
Move-Item -LiteralPath $from -Destination $to
$utf8=New-Object Text.UTF8Encoding($false)
$corePath=Join-Path $sd $coreRel
$core=Get-Content -Raw -LiteralPath $corePath | ConvertFrom-Json
if([int64]$core.core.framework.hardware.cartridge_adapter -ne -1){throw 'Cartridge must remain disabled'}
$core.core.metadata.version=$Version
$core.core.metadata.date_release='2026-09-03'
[IO.File]::WriteAllText($corePath,($core | ConvertTo-Json -Depth 20),$utf8)
$basePrefix=[IO.Path]::GetFullPath($base).TrimEnd('\')+'\'
foreach($file in Get-ChildItem -LiteralPath $base -Recurse -File){
    $rel=$file.FullName.Substring($basePrefix.Length)
    if($rel -in @($appRel,$coreRel)){continue}
    $dest=if($rel -eq $oldInstance){$newInstance}else{$rel}
    if((Get-Sha $file.FullName) -ne (Get-Sha (Join-Path $sd $dest))){throw "Unexpected payload change: $rel"}
}
$readme=@"
ChisBurner - smoke.48 OPEN -> READ4 -> FLUSH control

Keep cartridge EMPTY. Merge ALL SD_ROOT, including Saves, into the TF root;
accept replacement, then power OFF/ON. Select "TF Read4 48.json" in the core.
Path: Assets/chisburner/ChisBurner.ChisBurner/TF Read4 48.json.
Screen MUST say "TF READ4 smoke.48". An older number means old app files.
Do not switch test entries without cold power: the recorder is single-use.
AUDIT BUILD/ARM FAIL stops before any file commands (wrong FPGA/API or arm error).

Only the app and launch identity changed. FPGA RBF and OS are BYTE-IDENTICAL
to the tested smoke47 package. No FPGA compile, OS relink or binary patch.
Same data.json, slot4 seed binding, output path and requested size 1048613.
The old diagnostic placeholder Saves/chisburner/common/storage-smoke.41.bin is
replaced by installing the package, then resized by OPEN. No cartridge save
is accessed. This is diagnostic, NOT a ROM dump or a confirmed FLUSH fix.

Sequence: arm diagnostics -> OS/PRE size checks -> OPEN -> POST size check ->
one SDK of_file_read_async(slot=4, offset=0, length=4) -> FLUSH only if read
completion succeeds. Ordinary POSIX reads are deliberately not used because
the OS file cache can satisfy them without a host command or prefetch a block.
The async SDK uses its existing uncached bounce buffer and completion callback.
No output_write, retries, cache warm-up commands added by this application,
or file command after FLUSH. Existing OS behavior itself is not changed.
There is no freeze or artificial settling delay between OPEN and READ/FLUSH;
the read wait polls SDK completion and yields for 1 ms between polls.

READ4 PASS means one matching successful SDK callback and no outstanding SDK
read or buffer-guard damage. It does NOT verify the four bytes against a source
file. BYTES are displayed in byte order for observation only. START/TOKEN and
CB/TOKEN/RC distinguish dispatch failure from completion failure. Read failure,
unsupported API, token mismatch, or unexpected metadata causes FLUSH SKIP.
No retry occurs, including on BUSY. The app's read wait has a 5-second watchdog
and 10000-poll fallback; the existing OS may return an earlier timeout. This
does not lengthen the unchanged blocking FLUSH timeout (~48 s in smoke47).
DMA and callback storage are static: late completion after a timeout cannot
write into an expired stack frame or restart the stopped sequence.

After the result, photograph the FIRST THREE pages: summary, OPEN+READ WINDOW,
FLUSH WINDOW. RIGHT advances, A returns to summary. If READ fails, FLUSH WINDOW
explicitly says NOT ISSUED; zero counts there are then expected.

The unchanged recorder has only two windows. OPEN+READ WINDOW includes OPEN,
the added READ, metadata and short screen-printing gaps. Its BU/OK counters
therefore include BOTH commands; O-CM counts only the OPEN 0192 command word.
FLUSH WINDOW begins at the actual FLUSH request; F-CM counts 0188. RD CMD SEEN
checks saved paired-TX events for 0180/0181 in the first window: NO means absent
from the saved records, not proof that no read request was made. P=O in later
event pages now means this combined OPEN+READ window.

TX Q/D/A = queued/completed/aborted local serial transfers. None proves correct
host decoding. STATUS WR and BU/OK observe actual host-to-core writes. SIZE NO
means no observed size reply, not a zero file size. TABLE RD is the count of
host reads from the size table, not CPU metadata access.
FLAGS: bit16 log full, bit17 event collision, bit18 armed, bit19 time saturated;
low16 event count. Counters continue even when events are lost. Ordinary FFFFFFFF
counters mean at least that value; LOST=FFFFFFFF conservatively means at least
FFFFFFF9. Timestamp raw clocks are 74.25 MHz, saturating after ~57.8 seconds;
event T64 uses clocks/64. Check flags before interpreting missing events.

Local checks: 286 read/control assertions and 100 unchanged SDK snapshot mock
cases, plus PC unsupported guard and RV32 SDK build. Mocks are not Pocket
firmware/filesystem or electrical simulation. Actual smoke48 result: PENDING.
FPGA inherits smoke47 constrained timing: setup +0.306 ns, hold +0.020 ns;
the platform is not fully constrained, so this is not proof of every pin/CDC.
"@
$provenance=@"
version=$Version
base=smoke47
os_byte_identical_smoke47=PASS
rbf_byte_identical_smoke47=PASS
all_other_SD_files_identical_except_app_core_version_instance_name=PASS
os_sha256=$(Get-Sha (Join-Path $sd $osRel))
os_internal_crc=$('{0:x8}' -f $crc.ComputedCrc)
os_length=$($crc.Length)
rbf_r_sha256=$(Get-Sha (Join-Path $sd $rbfRel))
app_sha256=$(Get-Sha $elf)
read_api=SDK_of_file_read_async
file_sdk_header_text_identical_to_integration=PASS
read_request=slot4_offset0_length4_once
read_control_host_checks=286_PASS
snapshot_host_cases=100_PASS
output_write_calls=0
retry_calls=0
on_read_failure=SKIP_FLUSH
post_flush_file_commands=0
instance=TF Read4 48
hardware_result=PENDING
"@
[IO.File]::WriteAllText((Join-Path $bundle 'README_TEST.txt'),$readme,$utf8)
[IO.File]::WriteAllText((Join-Path $bundle 'BUILD_PROVENANCE.txt'),$provenance,$utf8)
Copy-Item -LiteralPath (Join-Path $baseBundle 'BUILD_PROVENANCE.txt') -Destination (Join-Path $bundle 'BASE_SMOKE47_PROVENANCE.txt')
Copy-Item -LiteralPath (Join-Path $baseBundle 'FPGA_SOURCE_SHA256.txt') -Destination $bundle
$software=@('app\storage_read_control\main.c','app\storage_read_control\probe.h',
    'app\storage_read_control\read4.h','app\storage_read_control\test_probe.c',
    'app\storage_audit_probe\probe.h','app\storage_flush_probe\probe.h',
    '.integration\openfpgaos\src\firmware\api\of_file.h',
    '.integration\openfpgaos\src\firmware\api\of_bridge_audit.h',
    '.integration\openfpgaos\src\firmware\api\of_bridge_trace.h','tests\build-storage-read-control.ps1')
$sums=$software | ForEach-Object {"$(Get-Sha (Join-Path $root $_))  $_"}
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
    foreach($name in $hashes.Keys){
        $entry=$archive.GetEntry($name);if($null -eq $entry){throw "ZIP missing $name"}
        $stream=$entry.Open();$sha=[Security.Cryptography.SHA256]::Create()
        try {
            $h=[BitConverter]::ToString($sha.ComputeHash($stream)).Replace('-','').ToLowerInvariant()
            if($h -ne $hashes[$name]){throw "ZIP mismatch: $name"}
        } finally {$stream.Dispose();$sha.Dispose()}
    }
} finally {$archive.Dispose()}
"ZIP CRC and all payloads PASS: $zipPath"
"OS and FPGA byte-identical smoke47: PASS"
"ZIP SHA256: $(Get-Sha $zipPath)"
