[CmdletBinding()]
param()
$ErrorActionPreference='Stop'
. (Join-Path $PSScriptRoot 'os-image-integrity.ps1')
function Get-Sha([string]$Path){(Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant()}
function Get-BytesSha([byte[]]$Bytes){
    $sha=[Security.Cryptography.SHA256]::Create()
    try {return [BitConverter]::ToString($sha.ComputeHash($Bytes)).Replace('-','').ToLowerInvariant()}
    finally {$sha.Dispose()}
}
function Get-PayloadMap([string]$Directory){
    $prefix=[IO.Path]::GetFullPath($Directory).TrimEnd('\')+'\'
    $map=@{}
    foreach($f in Get-ChildItem -LiteralPath $Directory -Recurse -File){
        $rel=$f.FullName.Substring($prefix.Length).Replace('\','/')
        $map[$rel]=Get-Sha $f.FullName
    }
    return $map
}
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$baseBundle=Join-Path $root 'build\smoke\ChisBurner-storage-flush-0.1.0-smoke.49-snapshot-diag'
$base=Join-Path $baseBundle 'SD_ROOT'
$bundle=Join-Path $root 'build\smoke\ChisBurner-storage-flush-smoke.49B-fullfile-control'
$zipPath="$bundle.zip"
if((Test-Path -LiteralPath $bundle) -or (Test-Path -LiteralPath $zipPath)){throw 'Preserve existing package'}
if((Get-Sha "$baseBundle.zip") -ne '47c2e150a69a40ade7bc04a4a255e7012a4e45de13475d23a51e222c957bea5d'){
    throw 'Base smoke49 ZIP identity changed'
}
$osRel='Assets/chisburner/common/os.bin'
$appRel='Assets/chisburner/common/chisburnersmoke.elf'
$rbfRel='Cores/ChisBurner.ChisBurner/chis.rbf_r'
$fixtureRel='Saves/chisburner/common/storage-smoke.41.bin'
$pins=@{
    $osRel='17c8cae47bd24869e5fbaa01514ffcd0f5ffc77fd514f5305bd3eeb1a719f7fa'
    $appRel='c15410a7ce05e84d3092932efb594de93ed6dc8fce49328a13767f598bf59e44'
    $rbfRel='b0c016f3c8956263aef31d8a934b366413a45885d0f7e72080bd605ae2197223'
}
$baseMap=Get-PayloadMap $base
foreach($rel in $pins.Keys){if($baseMap[$rel] -ne $pins[$rel]){throw "Base mismatch: $rel"}}
# Validate every base payload against the immutable, hash-pinned smoke49 ZIP.
$archive=[IO.Compression.ZipFile]::OpenRead("$baseBundle.zip")
try {
    $baseZipFiles=@($archive.Entries | Where-Object {$_.FullName.StartsWith('SD_ROOT/') -and !$_.FullName.EndsWith('/')})
    if($baseZipFiles.Count -ne $baseMap.Count){throw 'Base payload set changed'}
    foreach($entry in $baseZipFiles){
        $rel=$entry.FullName.Substring(8);$stream=$entry.Open();$sha=[Security.Cryptography.SHA256]::Create()
        try {
            $h=[BitConverter]::ToString($sha.ComputeHash($stream)).Replace('-','').ToLowerInvariant()
            if($h -ne $baseMap[$rel]){throw "Base folder/ZIP mismatch: $rel"}
        } finally {$stream.Dispose();$sha.Dispose()}
    }
} finally {$archive.Dispose()}
$original=[IO.File]::ReadAllBytes((Join-Path $base $fixtureRel))
if($original.Length -ne 1 -or $original[0] -ne 0){throw 'Expected original one-byte zero placeholder'}
$crc=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes((Join-Path $base $osRel)))
New-Item -ItemType Directory -Path $bundle | Out-Null
Copy-Item -LiteralPath $base -Destination $bundle -Recurse
$sd=Join-Path $bundle 'SD_ROOT'
$sdPrefix=[IO.Path]::GetFullPath($sd).TrimEnd('\')+'\'
$fixture=[IO.Path]::GetFullPath((Join-Path $sd $fixtureRel))
if(!$fixture.StartsWith($sdPrefix,[StringComparison]::OrdinalIgnoreCase)){throw 'Unsafe generated fixture target'}
# Same initial byte and all-zero content; only the file's length is varied.
$expanded=[byte[]]::new(1048613)
$fixtureSha=Get-BytesSha $expanded
[IO.File]::WriteAllBytes($fixture,$expanded)
if((Get-Item -LiteralPath $fixture).Length -ne 1048613 -or (Get-Sha $fixture) -ne $fixtureSha){
    throw 'Full-length fixture verification failed'
}
$newMap=Get-PayloadMap $sd
if($newMap.Count -ne $baseMap.Count){throw 'Unexpected added/removed SD payload'}
$changed=@()
foreach($rel in $baseMap.Keys){
    if(!$newMap.ContainsKey($rel)){throw "Missing SD payload: $rel"}
    if($newMap[$rel] -ne $baseMap[$rel]){$changed+=$rel}
}
if($changed.Count -ne 1 -or $changed[0] -ne $fixtureRel){throw "Not a one-file control: $changed"}
if((Get-PayloadMap $base)[$fixtureRel] -ne $baseMap[$fixtureRel]){throw 'Original placeholder was changed'}
$fixtureMd5=(Get-FileHash -LiteralPath $fixture -Algorithm MD5).Hash.ToLowerInvariant()
$utf8=New-Object Text.UTF8Encoding($false)
$readme=@"
ChisBurner - smoke.49B FULL-FILE DATA-ONLY CONTROL

This is NOT a new ELF, OS or FPGA build. The on-screen title deliberately
remains TF SNAP smoke.49 and the launch entry remains TF Snapshot 49.json.
There is exactly ONE changed file inside SD_ROOT compared with smoke49:
  $fixtureRel
Old length: 1 byte. New length: 1,048,613 bytes. Content: all zero bytes.
The existing first byte is unchanged. All other SD files are byte-identical,
including core.json, data.json, launch JSON, INI, slot4 seed, ELF, OS and RBF.

INSTALL / VERIFY BEFORE BOOT
1. Keep the cartridge slot EMPTY.
2. Merge ALL contents of SD_ROOT into the TF card root; accept replacement.
   In particular, replace the file in Saves/chisburner/common, not only Assets
   and Cores. This replaces only our disposable storage diagnostic file.
3. BEFORE booting, check the file's SIZE (not Size on disk) on the TF card:
   storage-smoke.41.bin MUST be exactly 1,048,613 bytes. If it is still 1 byte,
   the control did not take effect. Verify on the TF, not just inside this ZIP.
   The unchanged screen title and POST 1048613 cannot verify this replacement.
4. Power OFF/ON and select TF Snapshot 49.json. No new launcher is added.
5. Send page1 (summary). If READ4 RC 0, also send pages4 and5 (OPEN+READ and
   FLUSH windows). If READ4 still fails, send pages3 and4 along with page1.
   RIGHT advances; A returns to summary. Do not switch instances mid-session.

You do not need to mount the TF for the assistant. Only copy/verify the package
yourself as usual. If you repeat the control later, recopy this full-length
fixture before the next cold boot: the existing OPEN request may alter it.
Do not reinstall an older package afterward: it restores the one-byte fixture.

Expected file SHA256 before boot: $fixtureSha
Expected file MD5 before boot:   $fixtureMd5
If using PowerShell, the optional Get-FileHash command is read-only; use the
actual TF drive/path and compare with these values. File size alone distinguishes
the original one-byte placeholder but does not verify every byte's contents.

WHY THIS CONTROL
In smoke49, OPEN succeeded, READ4 received one matching callback with rc=-2,
and the trace captured a real host write of 6F6B0002 to F8001000. Snapshot
readback and its header passed. FLUSH was skipped after the read failure.
The shipped placeholder was one byte, whereas the application requested four.
POST 1048613 is the CPU-updated slot table, not independent proof of the real
file length. Failure to expand the actual file is a hypothesis, NOT established.
This control supplies sufficient file length before OPEN without changing any
executable code, request sequence, API, destination, timing logic or timeout.

UNCHANGED SEQUENCE
OPEN -> check POST table -> one async READ of slot4, offset0, length4 -> FLUSH
only if the read completes successfully. No output_write and no read retry.
On read failure FLUSH is skipped; otherwise the existing blocking FLUSH may
still time out after about48 seconds. Wait for the result rather than treating
that known timeout window as a new boot failure. No cartridge operation occurs.

INTERPRETATION
- READ4 RC 0 and FLUSH RC 0: this test sequence completed, but no write/readback
  or full-ROM-dump correctness is established. The app does not compare bytes.
- READ4 RC 0 but FLUSH fails: the read control now succeeds; FLUSH remains an
  independent unresolved problem. Photograph pages1,4,5.
- READ4 still fails: pre-sizing alone was insufficient. OPEN may have altered
  the file or other causes may remain; do not conclude anything from POST alone.
- SNAPSHOT VALIDATED only refers to diagnostic readback, not file-operation success.

All-zero padding compresses well, so ZIP size is not the test file's size.
Local checks validate the pinned base archive, every payload, exact fixture
length/hash and the one-file-only difference, plus the internal OS CRC and
all ZIP payload hashes. No compiler or hardware simulator ran for this control:
the tested smoke49 executables are reused unchanged. Hardware result: PENDING.
"@
$provenance=@"
package=smoke.49B-fullfile-control
base=0.1.0-smoke.49-snapshot-diag
base_zip_sha256=47c2e150a69a40ade7bc04a4a255e7012a4e45de13475d23a51e222c957bea5d
on_screen_title=TF SNAP smoke.49
instance=TF Snapshot 49.json
exactly_one_changed_sd_file=PASS
changed_sd_file=$fixtureRel
original_fixture_bytes=1
new_fixture_bytes=1048613
fixture_content=all_zero
fixture_sha256=$fixtureSha
fixture_md5=$fixtureMd5
elf_byte_identical_smoke49=PASS
os_byte_identical_smoke49=PASS
rbf_byte_identical_smoke49=PASS
core_data_ini_launcher_and_all_other_files_identical=PASS
elf_sha256=$($newMap[$appRel])
os_sha256=$($newMap[$osRel])
rbf_sha256=$($newMap[$rbfRel])
os_internal_crc=$('{0:x8}' -f $crc.ComputedCrc)
compiler_invocations=0
hardware_simulations=0
output_write_calls=0
read_retry_calls=0
on_read_failure=SKIP_FLUSH
read_content_verified_by_app=false
hardware_result=PENDING
"@
[IO.File]::WriteAllText((Join-Path $bundle 'README_TEST.txt'),$readme,$utf8)
[IO.File]::WriteAllText((Join-Path $bundle 'BUILD_PROVENANCE.txt'),$provenance,$utf8)
Copy-Item -LiteralPath (Join-Path $baseBundle 'BUILD_PROVENANCE.txt') -Destination (Join-Path $bundle 'BASE_SMOKE49_PROVENANCE.txt')
Copy-Item -LiteralPath (Join-Path $baseBundle 'SOFTWARE_SOURCE_SHA256.txt') -Destination (Join-Path $bundle 'UNCHANGED_SMOKE49_SOFTWARE_SHA256.txt')
Copy-Item -LiteralPath (Join-Path $baseBundle 'FPGA_SOURCE_SHA256.txt') -Destination $bundle
$sums=$newMap.Keys | Sort-Object | ForEach-Object {"$($newMap[$_])  $_"}
[IO.File]::WriteAllLines((Join-Path $bundle 'SHA256SUMS.txt'),[string[]]$sums,$utf8)
$delta=@("ONLY_CHANGED_SD_FILE=$fixtureRel","BEFORE_BYTES=1","AFTER_BYTES=1048613",
    "BEFORE_SHA256=$($baseMap[$fixtureRel])","AFTER_SHA256=$fixtureSha",
    "ALL_OTHER_$($newMap.Count-1)_SD_FILES_BYTE_IDENTICAL=PASS")
[IO.File]::WriteAllLines((Join-Path $bundle 'PAYLOAD_DIFF.txt'),[string[]]$delta,$utf8)
[IO.File]::WriteAllText((Join-Path $bundle 'BUILDER_SHA256.txt'),"$(Get-Sha $PSCommandPath)  tests/build-storage-fullfile-control.ps1",$utf8)
Compress-Archive -Path (Join-Path $bundle '*') -DestinationPath $zipPath -CompressionLevel Optimal
$null=Assert-ChisBurnerZipOsIntegrity -ZipPath $zipPath
$archive=[IO.Compression.ZipFile]::OpenRead($zipPath)
try {
    $payloads=@($archive.Entries | Where-Object {$_.FullName.StartsWith('SD_ROOT/') -and !$_.FullName.EndsWith('/')})
    if($payloads.Count -ne $newMap.Count){throw 'ZIP payload set mismatch'}
    foreach($entry in $payloads){
        $rel=$entry.FullName.Substring(8);$stream=$entry.Open();$sha=[Security.Cryptography.SHA256]::Create()
        try {
            $h=[BitConverter]::ToString($sha.ComputeHash($stream)).Replace('-','').ToLowerInvariant()
            if($h -ne $newMap[$rel]){throw "ZIP payload mismatch: $rel"}
            if($rel -eq $fixtureRel -and $entry.Length -ne 1048613){throw 'ZIP fixture length mismatch'}
        } finally {$stream.Dispose();$sha.Dispose()}
    }
} finally {$archive.Dispose()}
"Single-file-only SD difference: PASS ($($newMap.Count) files checked)"
"Full-length zero fixture: 1048613 bytes; SHA256 $fixtureSha"
"Unchanged ELF/OS/FPGA, OS CRC and ZIP payloads: PASS"
"ZIP: $zipPath"
"ZIP SHA256: $(Get-Sha $zipPath)"
