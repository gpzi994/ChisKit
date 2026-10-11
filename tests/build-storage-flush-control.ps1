[CmdletBinding()]
param(
    [string]$Distro = 'Ubuntu-24.04',
    [string]$OpenFpgaSdk = '/home/lin13/work/ONS_FPGA/openfpgaSDK',
    [string]$Version = '0.1.0-smoke.43-no-write'
)
$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'os-image-integrity.ps1')

function Convert-ToWslPath([string]$Path) {
    $full = [System.IO.Path]::GetFullPath($Path)
    if ($full -notmatch '^([A-Za-z]):\\(.*)$') {
        throw "Expected a local Windows drive path: $full"
    }
    return '/mnt/' + $Matches[1].ToLowerInvariant() + '/' + $Matches[2].Replace('\', '/')
}
function Get-Sha([string]$Path) {
    return (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant()
}

if ($Version -notmatch '^0\.1\.0-smoke\.43-[a-z0-9-]+$' -or $Version.Length -gt 31) {
    throw 'Invalid smoke.43 version'
}
$projectRoot = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$buildRoot = Join-Path $projectRoot 'build\smoke'
$baseSd = Join-Path $buildRoot 'ChisBurner-storage-stream-0.1.0-smoke.42-crc-control\SD_ROOT'
$bundleRoot = Join-Path $buildRoot "ChisBurner-storage-flush-$Version"
$zipPath = "$bundleRoot.zip"
if ((Test-Path -LiteralPath $bundleRoot) -or (Test-Path -LiteralPath $zipPath)) {
    throw 'Output already exists; do not replace historical test evidence'
}
$osRelative = 'Assets\chisburner\common\os.bin'
$appRelative = 'Assets\chisburner\common\chisburnersmoke.elf'
$rbfRelative = 'Cores\ChisBurner.ChisBurner\chis.rbf_r'
$coreRelative = 'Cores\ChisBurner.ChisBurner\core.json'
$oldInstance = 'Assets\chisburner\ChisBurner.ChisBurner\TF Storage CRC 42.json'
$newInstance = 'Assets\chisburner\ChisBurner.ChisBurner\TF Flush NOWRITE 43.json'
$seedRelative = 'Saves\chisburner\common\storage-smoke.41.bin'
$critical = @{
    $osRelative = 'b627365bef8b297e5362bd00863c57cfc0507459aebf5d5379f1cd23c26d24e8'
    $rbfRelative = 'b3c3d8b013859b555dce81cf556c342b0ab8ccf460418d9b4b32f1642784dd4b'
    $appRelative = '344907d6871f48f92d8ea93e97bfc835386d65ec0de57688f5f5809488d353f9'
}
foreach ($relative in $critical.Keys) {
    if ((Get-Sha (Join-Path $baseSd $relative)) -ne $critical[$relative]) {
        throw "Smoke.42 source hash mismatch: $relative"
    }
}
$crc = Assert-ChisBurnerOsIntegrity -Bytes ([System.IO.File]::ReadAllBytes((Join-Path $baseSd $osRelative)))
$seed = [System.IO.File]::ReadAllBytes((Join-Path $baseSd $seedRelative))
if ($seed.Length -ne 1 -or $seed[0] -ne 0) { throw 'Unexpected diagnostic seed' }

$appRoot = Join-Path $projectRoot 'app\storage_flush_probe'
$main = Get-Content -Raw -LiteralPath (Join-Path $appRoot 'main.c')
$probe = Get-Content -Raw -LiteralPath (Join-Path $appRoot 'probe.h')
if (-not $main.Contains('TF FLUSH smoke.43') -or
    -not $probe.Contains('/Saves/chisburner/common/storage-smoke.41.bin')) {
    throw 'Application identity/path guard failed'
}
if (($main + $probe) -match 'of_cart_|0x400000|of_output_write\s*\(') {
    throw 'No cartridge, MMIO or output_write access permitted in this probe'
}
$appWsl = Convert-ToWslPath $appRoot
& wsl.exe -d $Distro -- bash -lc "cd '$appWsl' && make clean OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0 && make host-test OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0 && make OPENFPGA_SDK='$OpenFpgaSdk' USE_SDK_CONTAINER=0"
if ($LASTEXITCODE -ne 0) { throw 'Application build/test failed' }
$appElf = Join-Path $projectRoot 'build\storage-flush-app\app.elf'
if (-not (Test-Path -LiteralPath $appElf)) { throw 'Application ELF missing' }

New-Item -ItemType Directory -Path $bundleRoot | Out-Null
Copy-Item -LiteralPath $baseSd -Destination $bundleRoot -Recurse
$sdRoot = Join-Path $bundleRoot 'SD_ROOT'
Copy-Item -LiteralPath $appElf -Destination (Join-Path $sdRoot $appRelative) -Force
Move-Item -LiteralPath (Join-Path $sdRoot $oldInstance) -Destination (Join-Path $sdRoot $newInstance)
$corePath = Join-Path $sdRoot $coreRelative
$core = Get-Content -Raw -LiteralPath $corePath | ConvertFrom-Json
$core.core.metadata.version = $Version
$core.core.metadata.date_release = '2026-09-03'
$utf8 = New-Object System.Text.UTF8Encoding($false)
[System.IO.File]::WriteAllText($corePath, ($core | ConvertTo-Json -Depth 20), $utf8)

# Every payload except the application and core metadata must remain identical;
# the launch descriptor is renamed only, including its original contents.
$basePrefix = [System.IO.Path]::GetFullPath($baseSd).TrimEnd('\') + '\'
foreach ($file in Get-ChildItem -LiteralPath $baseSd -Recurse -File) {
    $relative = $file.FullName.Substring($basePrefix.Length)
    if ($relative -eq $appRelative -or $relative -eq $coreRelative) { continue }
    $dest = if ($relative -eq $oldInstance) { $newInstance } else { $relative }
    if ((Get-Sha $file.FullName) -ne (Get-Sha (Join-Path $sdRoot $dest))) {
        throw "Unintended payload change: $relative"
    }
}
$appHash = Get-Sha (Join-Path $sdRoot $appRelative)
$readme = @"
ChisBurner - smoke.43 OPEN -> FLUSH without WRITE

Keep the cartridge slot EMPTY. Merge the entire SD_ROOT into the SD root,
including Saves, and approve replacement. The included diagnostic seed replaces
storage-smoke.41.bin from the earlier storage test; it is not a cartridge save.
Power the Pocket off/on before the test to clear any earlier in-flight command.
Select "TF Flush NOWRITE 43". The screen MUST say "TF FLUSH smoke.43".
If it still says smoke.41, the application replacement did not take effect.
Do not interpret that as a result for this test; recopy the complete SD_ROOT.
Old launcher entries may remain after a merge; choose the new entry above.

Exact smoke.42 OS and RBF; no OS rebuild, code patch or FPGA rebuild.
The existing output-size patch is retained unchanged, not newly verified.
Only the application file-operation sequence changes:
  OPEN same diagnostic path, requesting 1048613 bytes
  FLUSH once, with zero output WRITE commands
No file-size query or deliberate delay is inserted. No retry or further file
command is issued if OPEN/FLUSH fails. OS timeout text remains on screen.
This may resize the diagnostic file; its contents are not a test payload.

Report OPEN, WRITE COMMANDS, FLUSH RC, and the full ACK/DONE timeout line if any.
The operation counter may differ from #73 because writes/app loading changed.
FLUSH PASS would show that the path can complete without prior output writes.
FLUSH failure would show the stream/tail write is not required to trigger it;
it would NOT alone identify the faulty handshake stage or prove a host bug.
This is a diagnostic control, NOT a verified FLUSH fix or a ROM dump test.

Local checks: sequence mocks, unsupported-PC fallback, RV32 SDK application
build, unchanged payload hashes, OS internal CRC, and every ZIP payload hash.
Mocks do not emulate Pocket firmware or its asynchronous bridge timings.
"@
$provenance = @"
version=$Version
variant=smoke42_app_only_no_output_write
base_package=ChisBurner-storage-stream-0.1.0-smoke.42-crc-control
os_sha256=$($critical[$osRelative])
rbf_sha256=$($critical[$rbfRelative])
app_sha256=$appHash
main_source_sha256=$(Get-Sha (Join-Path $appRoot 'main.c'))
probe_source_sha256=$(Get-Sha (Join-Path $appRoot 'probe.h'))
os_internal_crc=$('{0:x8}' -f $crc.ComputedCrc)
os_and_rbf_byte_identical_smoke42=PASS
os_or_fpga_rebuild=NO
output_write_calls=0
output_path=/Saves/chisburner/common/storage-smoke.41.bin
requested_size=1048613
sequence_mocks=6_PASS_NOT_HARDWARE_EMULATION
hardware_result=PENDING
"@
[System.IO.File]::WriteAllText((Join-Path $bundleRoot 'README_TEST.txt'), $readme, $utf8)
[System.IO.File]::WriteAllText((Join-Path $bundleRoot 'BUILD_PROVENANCE.txt'), $provenance, $utf8)
$prefix = [System.IO.Path]::GetFullPath($sdRoot).TrimEnd('\') + '\'
$payloadHashes = @{}
$sums = Get-ChildItem -LiteralPath $sdRoot -Recurse -File | Sort-Object FullName | ForEach-Object {
    $relative = $_.FullName.Substring($prefix.Length).Replace('\', '/')
    $hash = Get-Sha $_.FullName
    $payloadHashes["SD_ROOT/$relative"] = $hash
    "$hash  $relative"
}
[System.IO.File]::WriteAllLines((Join-Path $bundleRoot 'SHA256SUMS.txt'), [string[]]$sums, $utf8)
Compress-Archive -Path (Join-Path $bundleRoot '*') -DestinationPath $zipPath -CompressionLevel Optimal
$null = Assert-ChisBurnerZipOsIntegrity -ZipPath $zipPath
$archive = [System.IO.Compression.ZipFile]::OpenRead($zipPath)
try {
    foreach ($name in $payloadHashes.Keys) {
        $entry = $archive.GetEntry($name)
        if ($null -eq $entry) { throw "ZIP payload missing: $name" }
        $stream = $entry.Open()
        $sha = [System.Security.Cryptography.SHA256]::Create()
        try {
            $hash = [BitConverter]::ToString($sha.ComputeHash($stream)).Replace('-', '').ToLowerInvariant()
            if ($hash -ne $payloadHashes[$name]) { throw "ZIP hash mismatch: $name" }
        } finally { $sha.Dispose(); $stream.Dispose() }
    }
} finally { $archive.Dispose() }
Write-Output "ZIP payload hashes: PASS ($($payloadHashes.Count) files)"
Write-Output 'OS internal CRC and byte-identical smoke.42 OS/RBF: PASS'
Write-Output "Package: $zipPath"
Write-Output "ZIP SHA256: $(Get-Sha $zipPath)"
