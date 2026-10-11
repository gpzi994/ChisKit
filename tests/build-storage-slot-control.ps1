[CmdletBinding()]
param([string]$Distro = 'Ubuntu-24.04',
      [string]$OpenFpgaSdk = '/home/lin13/work/ONS_FPGA/openfpgaSDK',
      [string]$Version = '0.1.0-smoke.45-slot-control')
$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'os-image-integrity.ps1')
function Get-Sha([string]$Path) { (Get-FileHash -LiteralPath $Path).Hash.ToLowerInvariant() }
function Wsl-Path([string]$Path) {
    $full = [IO.Path]::GetFullPath($Path)
    if ($full -notmatch '^([A-Za-z]):\\(.*)$') { throw 'Expected local drive path' }
    '/mnt/' + $Matches[1].ToLowerInvariant() + '/' + $Matches[2].Replace('\','/')
}
if ($Version -notmatch '^0\.1\.0-smoke\.45-[a-z0-9-]+$' -or $Version.Length -gt 31) {
    throw 'Invalid smoke.45 version'
}
$root = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$baseSd = Join-Path $root 'build\smoke\ChisBurner-storage-flush-0.1.0-smoke.44-host-trace\SD_ROOT'
$bundle = Join-Path $root "build\smoke\ChisBurner-storage-flush-$Version"
$zipPath = "$bundle.zip"
if ((Test-Path -LiteralPath $bundle) -or (Test-Path -LiteralPath $zipPath)) { throw 'Preserve existing package' }
$osRel = 'Assets\chisburner\common\os.bin'
$appRel = 'Assets\chisburner\common\chisburnersmoke.elf'
$rbfRel = 'Cores\ChisBurner.ChisBurner\chis.rbf_r'
$coreRel = 'Cores\ChisBurner.ChisBurner\core.json'
$instanceDir = 'Assets\chisburner\ChisBurner.ChisBurner'
$oldInstance = "$instanceDir\TF Flush TRACE 44.json"
$instanceA = "$instanceDir\TF Flush A45 Unbound.json"
$instanceB = "$instanceDir\TF Flush B45 Bound.json"
$seedRel = 'Assets\chisburner\common\storage-seed45.bin'
$critical = @{
    $osRel = '45792a4fe6937403fd73a5efd1d4077f65e6c62454aa1100f851c9c2b80d4e1a'
    $rbfRel = '9f75bc14165687901fc2e777c2a8ec0a7c655ba6ada146586f53215e3f232978'
    $appRel = '34b808b223e5db02edf12e4550130e528dbfaf2ee4827a7e1b56c11cf523b770'
}
foreach ($rel in $critical.Keys) {
    if ((Get-Sha (Join-Path $baseSd $rel)) -ne $critical[$rel]) { throw "Smoke.44 base mismatch: $rel" }
}
$crc = Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes((Join-Path $baseSd $osRel)))
$appRoot = Join-Path $root 'app\storage_slot_probe'
$appWsl = Wsl-Path $appRoot
$source = (Get-Content -Raw -LiteralPath (Join-Path $appRoot 'main.c')) + (Get-Content -Raw -LiteralPath (Join-Path $appRoot 'probe.h'))
if ($source -match 'of_cart_|0x400000|of_output_write\s*\(') { throw 'Application must use services only, with no cartridge or output writes' }
& wsl.exe -d $Distro -- bash -lc "cd '$appWsl' && make clean OPENFPGA_SDK='$OpenFpgaSdk' && make host-test all OPENFPGA_SDK='$OpenFpgaSdk'"
if ($LASTEXITCODE -ne 0) { throw 'SDK application build/test failed' }
$elf = Join-Path $root 'build\storage-slot-app\app.elf'
if (-not [Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($elf)).Contains('TF SLOT smoke.45')) {
    throw 'ELF identity missing'
}
New-Item -ItemType Directory -Path $bundle | Out-Null
Copy-Item -LiteralPath $baseSd -Destination $bundle -Recurse
$sd = Join-Path $bundle 'SD_ROOT'
Copy-Item -LiteralPath $elf -Destination (Join-Path $sd $appRel) -Force
Move-Item -LiteralPath (Join-Path $sd $oldInstance) -Destination (Join-Path $sd $instanceA)
$a = Get-Content -Raw -LiteralPath (Join-Path $sd $instanceA) | ConvertFrom-Json
if (@($a.instance.data_slots).Count -ne 3 -or @($a.instance.data_slots | Where-Object id -eq 4).Count -ne 0) {
    throw 'A must preserve the original unbound slot 4 launch'
}
$b = Get-Content -Raw -LiteralPath (Join-Path $sd $instanceA) | ConvertFrom-Json
$b.instance.data_slots = @($b.instance.data_slots) + @([pscustomobject]@{id=4; filename='storage-seed45.bin'})
$utf8 = New-Object Text.UTF8Encoding($false)
[IO.File]::WriteAllText((Join-Path $sd $instanceB), ($b | ConvertTo-Json -Depth 20), $utf8)
[IO.File]::WriteAllBytes((Join-Path $sd $seedRel), [byte[]](0..63))
$data = Get-Content -Raw -LiteralPath (Join-Path $sd 'Cores\ChisBurner.ChisBurner\data.json') | ConvertFrom-Json
$slot4 = @($data.data.data_slots | Where-Object id -eq 4)
if ($slot4.Count -ne 1 -or -not $slot4[0].deferload -or ([int]$slot4[0].parameters -band 8)) {
    throw 'Slot 4 must remain writable and deferred'
}
$corePath = Join-Path $sd $coreRel
$core = Get-Content -Raw -LiteralPath $corePath | ConvertFrom-Json
if ([int64]$core.core.framework.hardware.cartridge_adapter -ne -1) { throw 'Cartridge must remain disabled' }
$core.core.metadata.version = $Version
$core.core.metadata.date_release = '2026-09-03'
[IO.File]::WriteAllText($corePath, ($core | ConvertTo-Json -Depth 20), $utf8)
# Both launch descriptors run the same application/OS/RBF. B differs ONLY by
# adding slot 4's initial filename. The deferred data definition is unchanged.
$bCheck = Get-Content -Raw -LiteralPath (Join-Path $sd $instanceB) | ConvertFrom-Json
$without4 = @($bCheck.instance.data_slots | Where-Object id -ne 4)
if (($without4 | ConvertTo-Json -Compress) -cne (@($a.instance.data_slots) | ConvertTo-Json -Compress)) {
    throw 'A/B changes more than the initial slot 4 binding'
}
$basePrefix = [IO.Path]::GetFullPath($baseSd).TrimEnd('\') + '\'
foreach ($f in Get-ChildItem -LiteralPath $baseSd -Recurse -File) {
    $rel = $f.FullName.Substring($basePrefix.Length)
    if ($rel -in @($appRel,$coreRel)) { continue }
    $dest = if ($rel -eq $oldInstance) { $instanceA } else { $rel }
    if ((Get-Sha $f.FullName) -ne (Get-Sha (Join-Path $sd $dest))) { throw "Unexpected payload change: $rel" }
}
$readme = @"
ChisBurner - smoke.45 slot binding A/B control

Keep the cartridge slot EMPTY. Merge ALL SD_ROOT, including Saves, and approve
replacement. This resets the old storage-smoke.41.bin diagnostic placeholder;
no cartridge save is used. Screen MUST say "TF SLOT smoke.45". If not, the
application was not replaced. New menu entries must also be present.

1. Power OFF/ON, select "TF Flush A45 Unbound"; record results.
2. Power OFF/ON again, select "TF Flush B45 Bound"; record results.
Do not switch instances without powering off: earlier in-flight state or
dynamic bindings would weaken the comparison. Old menu entries may remain.

A: same startup binding as smoke.44 (slots 1/2/3, no initial file for slot 4).
B: adds a 64-byte Assets/chisburner/common/storage-seed45.bin binding to slot 4.
Both then OPEN the SAME Saves diagnostic output, requesting 1048613 bytes,
query the SDK's slot 4 size, and FLUSH once without writing output data.
Slot sizes are queried through the existing SDK file_size service. In this
RBF the full 32-bit datatable register is implemented; no application MMIO.
No queries/retries/commands after FLUSH. If OPEN fails, no subsequent query
or FLUSH. If OS SLOT1 != 137480, output commands are skipped as a sanity guard.

Report for A and B: OS SLOT1, SLOT4 PRE/POST, OPEN, FLUSH RC, and complete
timeout ST if present. OS SLOT1 should be 137480. B PRE should be 64; after
successful OPEN the expected POST is 1048613. A PRE is an observation, not a
required PASS value. Size -1 means unresolved/zero-size/lookup failure; it
does NOT alone prove that the slot ID is absent or identify the cause.
Counter # may change because the additional bound asset affects OS startup.

OS and RBF are byte-for-byte smoke.44, retaining its handshake trace and its
existing output-size patch. No new OS patch, no FPGA compile, no delay change.
Hypothesis: initial slot binding/table state matters to FLUSH. Not established.
This is a diagnostic control, NOT a confirmed fix or a ROM dump test.

Local verification: 8 mocked sequences, unsupported-PC fallback, RV32 SDK
build, A/B configuration difference, unchanged OS/RBF and ZIP payload hashes,
and OS internal CRC. Hardware results pending; mocks do not emulate Pocket.
"@
$provenance = @"
version=$Version
variant=smoke44_app_only_slot_metadata_and_initial_binding_ab
os_sha256=$($critical[$osRel])
rbf_r_sha256=$($critical[$rbfRel])
os_and_rbf_byte_identical_smoke44=PASS
os_internal_crc=$('{0:x8}' -f $crc.ComputedCrc)
os_or_fpga_rebuild=NO
app_sha256=$(Get-Sha (Join-Path $sd $appRel))
app_main_sha256=$(Get-Sha (Join-Path $appRoot 'main.c'))
app_probe_sha256=$(Get-Sha (Join-Path $appRoot 'probe.h'))
instance_a=TF Flush A45 Unbound
instance_b=TF Flush B45 Bound
ab_difference=initial_slot4_binding_only
seed_size=64
output_path=/Saves/chisburner/common/storage-smoke.41.bin
requested_size=1048613
output_write_calls=0
post_flush_file_commands=0
mock_cases=8_PASS
hardware_result=PENDING
"@
[IO.File]::WriteAllText((Join-Path $bundle 'README_TEST.txt'), $readme, $utf8)
[IO.File]::WriteAllText((Join-Path $bundle 'BUILD_PROVENANCE.txt'), $provenance, $utf8)
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
Write-Output "A/B configuration, OS CRC and ZIP payload checks PASS: $zipPath"
Write-Output "ZIP SHA256: $(Get-Sha $zipPath)"
