param(
    [string]$ModelSimBin = "C:\intelFPGA_lite\20.1\modelsim_ase\win32aloem"
)

$ErrorActionPreference = "Stop"
$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$buildDir = Join-Path $repoRoot "build\modelsim"
$vlib = Join-Path $ModelSimBin "vlib.exe"
$vlog = Join-Path $ModelSimBin "vlog.exe"
$vsim = Join-Path $ModelSimBin "vsim.exe"

foreach ($tool in @($vlib, $vlog, $vsim)) {
    if (-not (Test-Path -LiteralPath $tool)) {
        throw "Missing ModelSim tool: $tool"
    }
}

New-Item -ItemType Directory -Force -Path $buildDir | Out-Null
Push-Location $buildDir
try {
    if (-not (Test-Path -LiteralPath (Join-Path $buildDir "work"))) {
        & $vlib work
        if ($LASTEXITCODE -ne 0) { throw "vlib failed with exit code $LASTEXITCODE" }
    }

    $sources = @(
        (Join-Path $repoRoot "core\cartridge_hal\pct_cart_guard.sv"),
        (Join-Path $repoRoot "core\cartridge_hal\pct_cart_safe_pins.sv"),
        (Join-Path $repoRoot "core\platform\openfpga\pct_apf_host_port.sv"),
        (Join-Path $repoRoot "core\platform\openfpga\pct_apf_target_port.sv"),
        (Join-Path $repoRoot "core\storage\pct_storage_sm.sv"),
        (Join-Path $repoRoot "core\storage\pct_storage_buffers.sv"),
        (Join-Path $repoRoot "core\pct_core_skeleton.sv"),
        (Join-Path $repoRoot "core\platform\openfpga\core_top.sv"),
        (Join-Path $repoRoot "tests\rtl\tb_pct_cart_guard.sv"),
        (Join-Path $repoRoot "tests\rtl\tb_pct_core_storage.sv"),
        (Join-Path $repoRoot "tests\rtl\tb_core_top_safe.sv")
    )

    # ModelSim 2020.1 reports ANSI SystemVerilog `input logic` under
    # `default_nettype none` as suppressible diagnostic 2892. The declaration
    # is valid SystemVerilog; suppress only that legacy front-end diagnostic.
    & $vlog -sv -suppress 2892 @sources
    if ($LASTEXITCODE -ne 0) { throw "vlog failed with exit code $LASTEXITCODE" }

    foreach ($testbench in @("tb_pct_cart_guard", "tb_pct_core_storage", "tb_core_top_safe")) {
        & $vsim -c -quiet "work.$testbench" -do "run -all; quit -f"
        if ($LASTEXITCODE -ne 0) { throw "$testbench failed with exit code $LASTEXITCODE" }
    }
} finally {
    Pop-Location
}
