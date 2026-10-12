$ErrorActionPreference = 'Stop'
$root = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..')).Replace('\','/')
$source = "$root/build/gb-compare208-quartus"
$fit = "$root/build/boot-splash209-quartus"
$os = "$root/.integration/openfpgaos/src/firmware/os/bld/chiskit-splash209"
$q = 'C:/altera_lite/25.1std/quartus/bin64'
if (Test-Path $fit) { throw 'Preserve existing build; inspect it before retrying' }
if ((Get-Content "$source/output_files/ap_core.fit.summary" -Raw) -notmatch 'Fitter Status : Successful') { throw 'Missing routed baseline' }
Copy-Item -LiteralPath $source -Destination $fit -Recurse
# The fitted database retains the absolute MIF input path. Temporarily supply
# its matching new image there; never alter the original fitted database.
Copy-Item "$source/firmware.mif" "$fit/original-release-firmware.mif"
$oldHash = (Get-FileHash "$source/firmware.mif").Hash
try {
    Copy-Item "$os/firmware.mif" "$source/firmware.mif" -Force
    Copy-Item "$os/firmware.mif" "$fit/firmware.mif" -Force
    Push-Location $fit
    try {
        $ErrorActionPreference = 'Continue'
        & "$q/quartus_cdb.exe" ap_core --update_mif *> update-mif209.log
        if ($LASTEXITCODE) { throw 'MIF update failed' }
        & "$q/quartus_sta.exe" ap_core *> timing209.log
        if ($LASTEXITCODE) { throw 'Timing analysis failed' }
        & "$q/quartus_sta.exe" -t "$PSScriptRoot/check-sdram124-sta.tcl" $fit *> detail209.log
        if ($LASTEXITCODE) { throw 'Detailed timing analysis failed' }
        $ErrorActionPreference = 'Stop'
        & "$PSScriptRoot/check-sdram124-reports.ps1" -reportRoot $fit -AllowUnusedBridgeReady
        $ErrorActionPreference = 'Continue'
        & "$q/quartus_asm.exe" ap_core *> assembly209.log
        if ($LASTEXITCODE) { throw 'Assembly failed' }
        $ErrorActionPreference = 'Stop'
    } finally { Pop-Location }
} finally {
    Copy-Item "$fit/original-release-firmware.mif" "$source/firmware.mif" -Force
    if ((Get-FileHash "$source/firmware.mif").Hash -ne $oldHash) { throw 'Baseline restore failed' }
}
if ((Get-FileHash "$fit/firmware.mif").Hash -ne (Get-FileHash "$os/firmware.mif").Hash) { throw 'OS/BRAM mismatch' }
Write-Host "PASS paired OS/BRAM assembled in $fit; hardware boot remains unverified."
