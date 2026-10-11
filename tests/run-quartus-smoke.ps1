param(
    [string]$QuartusBin
)

$ErrorActionPreference = "Stop"

if ([string]::IsNullOrWhiteSpace($QuartusBin)) {
    $installRoots = @(
        "C:\altera_lite",
        "C:\intelFPGA_lite",
        "D:\altera_lite",
        "D:\intelFPGA_lite"
    )
    $candidates = @()

    foreach ($installRoot in $installRoots) {
        if (Test-Path -LiteralPath $installRoot) {
            $versions = Get-ChildItem -LiteralPath $installRoot -Directory |
                Sort-Object Name -Descending
            foreach ($version in $versions) {
                $candidates += Join-Path $version.FullName "quartus\bin64"
            }
        }
    }

    foreach ($candidate in $candidates) {
        $candidateQuartusRoot = (Join-Path $candidate "..")
        $candidateMap = Join-Path $candidate "quartus_map.exe"
        $candidateCycloneV = Join-Path $candidateQuartusRoot "common\devinfo\cyclonev"
        if ((Test-Path -LiteralPath $candidateMap) -and
            (Test-Path -LiteralPath $candidateCycloneV)) {
            $QuartusBin = $candidate
            break
        }
    }

    if ([string]::IsNullOrWhiteSpace($QuartusBin)) {
        throw "No Quartus installation with a Cyclone V device database was found"
    }
}

$quartusMap = Join-Path $QuartusBin "quartus_map.exe"
if (-not (Test-Path -LiteralPath $quartusMap)) {
    throw "Missing Quartus tool: $quartusMap"
}

$quartusRoot = Join-Path $QuartusBin ".."
$cycloneVDatabase = Join-Path $quartusRoot "common\devinfo\cyclonev"
if (-not (Test-Path -LiteralPath $cycloneVDatabase)) {
    throw "Quartus at $QuartusBin does not include the Cyclone V device database"
}

Write-Host "Using Quartus: $quartusMap"
Write-Host "Cyclone V database: $cycloneVDatabase"

Push-Location $PSScriptRoot
try {
    & $quartusMap quartus_smoke --read_settings_files=on --write_settings_files=off
    if ($LASTEXITCODE -ne 0) {
        throw "Quartus analysis and synthesis failed with exit code $LASTEXITCODE"
    }
} finally {
    Pop-Location
}
