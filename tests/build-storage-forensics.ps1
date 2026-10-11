[CmdletBinding()]
param([string]$Distro='Ubuntu-24.04',
      [string]$OpenFpgaSdk='/home/lin13/work/ONS_FPGA/openfpgaSDK',
      [string]$Version='0.1.0-smoke.49-snapshot-diag')
$ErrorActionPreference='Stop'
. (Join-Path $PSScriptRoot 'os-image-integrity.ps1')
function Get-Sha([string]$Path){(Get-FileHash -LiteralPath $Path).Hash.ToLowerInvariant()}
if($Version -notmatch '^0\.1\.0-smoke\.49-[a-z0-9-]+$' -or $Version.Length -gt 31){throw 'Invalid version'}
$root=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$baseBundle=Join-Path $root 'build\smoke\ChisBurner-storage-flush-0.1.0-smoke.48-read4-control'
$base=Join-Path $baseBundle 'SD_ROOT'
$bundle=Join-Path $root "build\smoke\ChisBurner-storage-flush-$Version"
$zipPath="$bundle.zip"
if((Test-Path -LiteralPath $bundle) -or (Test-Path -LiteralPath $zipPath)){throw 'Preserve existing package'}
$osRel='Assets\chisburner\common\os.bin'
$appRel='Assets\chisburner\common\chisburnersmoke.elf'
$rbfRel='Cores\ChisBurner.ChisBurner\chis.rbf_r'
$coreRel='Cores\ChisBurner.ChisBurner\core.json'
$oldInstance='Assets\chisburner\ChisBurner.ChisBurner\TF Read4 48.json'
$newInstance='Assets\chisburner\ChisBurner.ChisBurner\TF Snapshot 49.json'
$critical=@{
    $osRel='17c8cae47bd24869e5fbaa01514ffcd0f5ffc77fd514f5305bd3eeb1a719f7fa'
    $rbfRel='b0c016f3c8956263aef31d8a934b366413a45885d0f7e72080bd605ae2197223'
    $appRel='3f8d7b89fd35c9cedd4dbbbaae5bb5ea26d8c0e528adf3cc5243aee1aec152bd'
}
foreach($rel in $critical.Keys){
    if((Get-Sha (Join-Path $base $rel)) -ne $critical[$rel]){throw "Base mismatch: $rel"}
}
foreach($line in Get-Content -LiteralPath (Join-Path $baseBundle 'SHA256SUMS.txt')){
    if($line -notmatch '^([0-9a-f]{64})  (.+)$'){throw 'Bad base manifest'}
    $expected=$Matches[1];$rel=$Matches[2]
    if((Get-Sha (Join-Path $base $rel)) -ne $expected){throw "Base file changed: $rel"}
}
# Also pin the unchanged file sequence/helpers and SDK API definitions used by 48.
foreach($line in Get-Content -LiteralPath (Join-Path $baseBundle 'SOFTWARE_SOURCE_SHA256.txt')){
    if($line -notmatch '^([0-9a-f]{64})  (.+)$'){throw 'Bad base source manifest'}
    $expected=$Matches[1];$rel=$Matches[2]
    if((Get-Sha (Join-Path $root $rel)) -ne $expected){throw "smoke48 source changed: $rel"}
}
$crc=Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes((Join-Path $base $osRel)))
$appRoot=Join-Path $root 'app\storage_snapshot_forensics'
if($appRoot -notmatch '^([A-Za-z]):\\(.*)$'){throw 'Expected local drive path'}
$appWsl='/mnt/'+$Matches[1].ToLowerInvariant()+'/'+$Matches[2].Replace('\','/')
$source=(Get-Content -Raw -LiteralPath (Join-Path $appRoot 'main.c'))+
        (Get-Content -Raw -LiteralPath (Join-Path $appRoot 'snapshot.h'))
if($source -match 'of_cart_|0x40000|of_output_write\s*\(|\bfopen\s*\(|\bread\s*\('){throw 'No app MMIO, cached read or writes allowed'}
& wsl.exe -d $Distro -- bash -lc "cd '$appWsl' && diff --strip-trailing-cr '$OpenFpgaSdk/src/sdk/include/of_file.h' ../../.integration/openfpgaos/src/firmware/api/of_file.h && make clean OPENFPGA_SDK='$OpenFpgaSdk' && make host-test all OPENFPGA_SDK='$OpenFpgaSdk'"
if($LASTEXITCODE -ne 0){throw 'SDK application build/tests failed'}
$build=Join-Path $root 'build\storage-snapshot-forensics-app'
$elf=Join-Path $build 'app.elf'
if(![Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($elf)).Contains('TF SNAP smoke.49')){throw 'Wrong app identity'}
$sanCmd="cd '$appWsl' && cc -DOF_PC -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined -fno-omit-frame-pointer -I../../.integration/openfpgaos/src/firmware/api test_snapshot.c -o ../../build/storage-snapshot-forensics-app/snapshot-sanitized && ../../build/storage-snapshot-forensics-app/snapshot-sanitized && cc -DOF_PC -O1 -g -Wall -Wextra -Werror -fsanitize=address,undefined -fno-omit-frame-pointer -I../../.integration/openfpgaos/src/firmware/api ../storage_read_control/test_probe.c -o ../../build/storage-snapshot-forensics-app/sequence-sanitized && ../../build/storage-snapshot-forensics-app/sequence-sanitized"
& wsl.exe -d $Distro -- bash -lc $sanCmd
if($LASTEXITCODE -ne 0){throw 'ASan/UBSan checks failed'}
$demo=@(& wsl.exe -d $Distro -- bash -lc "cd '$appWsl' && ../../build/storage-snapshot-forensics-app/pc-fallback-test --demo")
if($LASTEXITCODE -ne 0){throw 'UI fixture failed'}
$demo=$demo | ForEach-Object {[regex]::Replace([string]$_,'\x1b\[[0-9;]*[A-Za-z]','')}
$rows=0;$pages=0;$maxRows=0;$maxWidth=0
foreach($line in $demo){
    if($line -match '^ TF SNAP smoke\.49 '){
        if($rows -gt 18){throw "UI page has $rows rows"}
        $maxRows=[Math]::Max($maxRows,$rows);$rows=0;++$pages
    }
    ++$rows;$maxWidth=[Math]::Max($maxWidth,$line.Length)
    if($line.Length -gt 40){throw "UI line exceeds 40 columns: $line"}
}
$maxRows=[Math]::Max($maxRows,$rows)
if($maxRows -gt 18 -or $pages -lt 10){throw 'UI pagination/height failure'}
foreach($expected in @('STAGE HEADER-CHECK','CAP 37 RAW GROUPS 37','SNAP RC -1 API 0',
                       'STAGE HEADER-READ','FAILED DATA WORD: 2','API -17 REG 3 CALL 6')){
    if(!($demo -contains $expected)){throw "Missing UI diagnostic: $expected"}
}
$utf8=New-Object Text.UTF8Encoding($false)
New-Item -ItemType Directory -Path $bundle | Out-Null
Copy-Item -LiteralPath $base -Destination $bundle -Recurse
$sd=Join-Path $bundle 'SD_ROOT'
$sdPrefix=[IO.Path]::GetFullPath($sd).TrimEnd('\')+'\'
Copy-Item -LiteralPath $elf -Destination (Join-Path $sd $appRel)
$from=[IO.Path]::GetFullPath((Join-Path $sd $oldInstance))
$to=[IO.Path]::GetFullPath((Join-Path $sd $newInstance))
if(!$from.StartsWith($sdPrefix) -or !$to.StartsWith($sdPrefix)){throw 'Unsafe staging move'}
Move-Item -LiteralPath $from -Destination $to
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
ChisBurner - smoke.49 snapshot forensics

Keep cartridge EMPTY. Merge ALL SD_ROOT folders into the TF root and accept
replacement. This replaces only project diagnostic files, including the old
Saves/chisburner/common/storage-smoke.41.bin placeholder; no cartridge save is used.
Power OFF/ON, then select TF Snapshot 49.json in the ChisBurner core.
Path: Assets/chisburner/ChisBurner.ChisBurner/TF Snapshot 49.json.
Screen MUST say TF SNAP smoke.49. An older number means the ELF replacement
did not take effect. Old launch entries may remain after merging; use 49.
Do not switch test entries without cold power: the recorder is single-use.

Please photograph ONLY the FIRST THREE pages initially:
1. Result summary: READ callback, FLUSH issued/skipped, SNAP/API/STAGE,
   FLAGS, captured count (CAP), successfully read raw groups, word counts.
2. Raw header words H0-H7 alongside expected values and the bad-field mask.
3. Exact failure stage, SDK return code, selector/call, failed DATA word,
   first mismatched header field, raw FREEZE/LEGACY and last successful words.
RIGHT advances; A returns to summary. No TF mounting/log export is required.

This changes ONLY the diagnostic application and its launch/version labels.
OS and FPGA bytes are identical to smoke48 (and smoke47). No FPGA compile,
OS rebuild/patch, new MMIO access, file write, or read retry was added.
The same smoke48 code still performs OPEN -> one SDK async READ4 -> FLUSH
only after read success. Read failure skips FLUSH; snapshot collection runs
after the stopped sequence. A successful READ callback alone does NOT compare
the four bytes with a known source file. This is NOT a ROM dump or a FLUSH fix.
The original read watchdog and existing OS FLUSH timeout remain unchanged.

Snapshot changes:
- Keep the raw COUNT flags separately from successfully read words. N0 is no
  longer used as an ambiguous substitute for the observed captured count.
- STAGE names the failing operation; API is its SDK return code. For a local
  header validation failure SNAP=-1, API=0, STAGE=HEADER-CHECK. A service failure
  has its own API return code and HEADER-READ/PAYLOAD-READ/etc. stage.
- H0 magic, H1 version, H2 length, H3 count, H5 flags, H6 phase are exactly the
  six smoke48 checks. BAD mask bits correspond to header indices. H4 is ticks
  and H7 is reserved: both shown raw, neither is a new validity check.
- Even if those header checks fail, collect at most 72+4*CAP raw words, using
  the independently range-checked COUNT (CAP <=256), not the malformed header.
- Stop immediately on any SDK read error. DATA advances a hardware cursor, so
  there is NO retry, rewind, automatic resynchronization, or second snapshot.
  Words from failed calls are NOT published. Keep all earlier successful words.
- A group is merely four raw words after fixed header offset72. A raw group
  is NOT a verified event when header checks fail. No counters are interpreted
  until the whole 72-word header passes its checks.

Pages4/5 show OPEN+READ and FLUSH counters only with a valid header. These use
the unchanged recorder: the first window includes both OPEN and READ, while
the second starts only if FLUSH was actually issued. TX != confirmed host
decoding. Pages6+ show the complete or partial raw stream (24 words/page):
word indices0..71 are header/stats, index72+ is the nominal event area.
Each event has four words [kind/phase/ticks64, address, data, next-address].
Do not draw event/timing conclusions from unvalidated or misaligned raw data.
After an API error the failed word index is zero-based; its hardware pop may
already have occurred, which is why no continuation/retry is attempted.

FLAGS: bit16 log full,17 event collision,18 armed,19 timestamp saturation;
low16 captured count. Header validation does not mean no events were lost.
Raw word count equality means complete readback, not successful READ/FLUSH.
The recorder itself and its timing constraints are unchanged from smoke47.

Local verification: exhaustive SDK read failure injection at all1101 call
positions in a maximum-size snapshot, six header mismatch cases, bounded raw
retention, partial data, invalid counts, ready timeout and canaries; ASan/UBSan;
unchanged286 READ4/sequence assertions and100 legacy snapshot cases; PC fallback;
SDK RV32 app build; UI text fixtures <=40 columns and18 rows; payload/ZIP hashes.
These mocks are not Pocket hardware tests. smoke49 hardware result: PENDING.
UI fixtures are synthetic examples, NOT transcriptions of the smoke48 failure.
"@
$provenance=@"
version=$Version
base=smoke48
os_byte_identical_smoke48_and_smoke47=PASS
rbf_byte_identical_smoke48_and_smoke47=PASS
all_other_SD_files_identical_except_app_core_version_instance_name=PASS
os_sha256=$(Get-Sha (Join-Path $sd $osRel))
os_internal_crc=$('{0:x8}' -f $crc.ComputedCrc)
os_length=$($crc.Length)
rbf_r_sha256=$(Get-Sha (Join-Path $sd $rbfRel))
app_sha256=$(Get-Sha $elf)
read_control_source_identical_smoke48=PASS
read_request=slot4_offset0_length4_once
output_write_calls=0
read_retry_calls=0
on_read_failure=SKIP_FLUSH
post_sequence_file_commands=0
snapshot_data_retry_calls=0
snapshot_max_words=1096
snapshot_forensics_checks=1842300_PASS
all_snapshot_service_failure_positions=1101_PASS
read_control_checks=286_PASS
legacy_snapshot_cases=100_PASS
asan_ubsan=PASS
ui_fixture_pages=$pages
ui_max_columns=$maxWidth
ui_max_rows=$maxRows
instance=TF Snapshot 49
hardware_result=PENDING
"@
[IO.File]::WriteAllText((Join-Path $bundle 'README_TEST.txt'),$readme,$utf8)
[IO.File]::WriteAllText((Join-Path $bundle 'BUILD_PROVENANCE.txt'),$provenance,$utf8)
[IO.File]::WriteAllLines((Join-Path $bundle 'UI_FIXTURES_SYNTHETIC.txt'),[string[]]$demo,$utf8)
Copy-Item -LiteralPath (Join-Path $baseBundle 'BUILD_PROVENANCE.txt') -Destination (Join-Path $bundle 'BASE_SMOKE48_PROVENANCE.txt')
Copy-Item -LiteralPath (Join-Path $baseBundle 'FPGA_SOURCE_SHA256.txt') -Destination $bundle
$software=@('app\storage_snapshot_forensics\main.c','app\storage_snapshot_forensics\snapshot.h',
    'app\storage_snapshot_forensics\Makefile','app\storage_snapshot_forensics\test_snapshot.c',
    'app\storage_read_control\probe.h','app\storage_read_control\read4.h',
    'app\storage_read_control\test_probe.c','app\storage_flush_probe\probe.h',
    '.integration\openfpgaos\src\firmware\api\of_file.h',
    '.integration\openfpgaos\src\firmware\api\of_bridge_audit.h',
    '.integration\openfpgaos\src\firmware\api\of_bridge_trace.h','tests\build-storage-forensics.ps1')
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
"OS and FPGA byte-identical smoke48/smoke47: PASS"
"UI fixtures: $pages pages, max $maxWidth columns and $maxRows rows"
"ZIP SHA256: $(Get-Sha $zipPath)"
