param([switch]$Deploy, [ValidateSet('209b','209c')][string]$Variant='209b')
$ErrorActionPreference = 'Stop'
$root = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..')).Replace('\','/')
$fit = "$root/build/boot-splash209-quartus"
$osSuffix = if ($Variant -eq '209c') { '209c' } else { '209' }
$os = "$root/.integration/openfpgaos/src/firmware/os/bld/chiskit-splash$osSuffix"
$base = "$root/build/publish/ChisKit-0.01b/Firmware/SD_ROOT"
$dst = "$root/build/releases/ChisKit-0.01b-splash$Variant-test"
if (Test-Path $dst) { throw 'Preserve existing test package' }
foreach ($item in @(@('update-mif209.log','MIF/HEX Update was successful'), @('assembly209.log','Assembler was successful'))) {
    if ((Get-Content "$fit/$($item[0])" -Raw) -notmatch $item[1]) { throw "Missing success: $($item[0])" }
}
& "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $fit -AllowUnusedBridgeReady
if ((Get-FileHash "$fit/firmware.mif").Hash -ne (Get-FileHash "$os/firmware.mif").Hash) { throw 'OS/BRAM pairing failed' }
. "$PSScriptRoot/os-image-integrity.ps1"
$null = Assert-ChisBurnerOsIntegrity -Bytes ([IO.File]::ReadAllBytes("$os/os.bin"))
New-Item -ItemType Directory -Path $dst | Out-Null
Copy-Item -LiteralPath $base -Destination "$dst/SD_ROOT" -Recurse
Copy-Item "$os/os.bin" "$dst/SD_ROOT/Assets/chiskit/common/os.bin" -Force
Add-Type -TypeDefinition 'public static class ReverseSplash209 { public static byte[] Run(byte[] a) { for(int i=0;i<a.Length;i++){int x=a[i],y=0;for(int k=0;k<8;k++){y=(y<<1)|(x&1);x>>=1;}a[i]=(byte)y;}return a;} }'
$rbf = [ReverseSplash209]::Run([IO.File]::ReadAllBytes("$fit/output_files/ap_core.rbf"))
[IO.File]::WriteAllBytes("$dst/SD_ROOT/Cores/Alius.chiskit/chiskit.rbf_r", $rbf)
$changed = @('Assets/chiskit/common/os.bin','Cores/Alius.chiskit/chiskit.rbf_r')
$manifest = @(Get-ChildItem "$dst/SD_ROOT" -Recurse -File | Sort-Object FullName | ForEach-Object {
    $p = $_.FullName.Substring(("$dst/SD_ROOT").Length+1).Replace('\','/')
    $hash = (Get-FileHash -LiteralPath $_.FullName).Hash
    if ($p -notin $changed -and $hash -ne (Get-FileHash -LiteralPath "$base/$p").Hash) { throw "Unexpected change $p" }
    "$hash  $p"
})
[IO.File]::WriteAllLines("$dst/SHA256SUMS.txt", [string[]]$manifest)
Copy-Item "$root/docs/BOOT_SPLASH209B_TEST.md" "$dst/README.md"
Compress-Archive -Path "$dst/SD_ROOT/*" -DestinationPath "$dst/ChisKit-0.01b-splash$Variant-test.zip"
if ($Deploy) {
    if (!(Test-Path 'E:/Cores/Alius.chiskit/core.json')) { throw 'Pocket not mounted at expected target' }
    if ((Get-FileHash 'E:/Assets/chiskit/common/chiskit.elf').Hash -ne (Get-FileHash "$base/Assets/chiskit/common/chiskit.elf").Hash) { throw 'Unexpected installed application' }
    $backup = "$dst/pocket-backup-$(Get-Date -Format yyyyMMdd-HHmmss)"
    foreach ($p in $changed) {
        New-Item -ItemType Directory -Path (Split-Path "$backup/$p") -Force | Out-Null
        Copy-Item -LiteralPath "E:/$p" -Destination "$backup/$p"
        if ((Get-FileHash "E:/$p").Hash -ne (Get-FileHash "$backup/$p").Hash) { throw 'Backup verification failed' }
    }
    foreach ($p in $changed) {
        Copy-Item -LiteralPath "$dst/SD_ROOT/$p" -Destination "E:/$p" -Force
        if ((Get-FileHash "E:/$p").Hash -ne (Get-FileHash "$dst/SD_ROOT/$p").Hash) { throw 'USB readback failed' }
    }
    Write-Host "PASS paired files copied and read back; backup $backup"
}
Write-Host "Package $dst/ChisKit-0.01b-splash$Variant-test.zip"
