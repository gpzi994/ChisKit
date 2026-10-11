#!/usr/bin/env bash
set -euo pipefail
project_root=$(cd "$(dirname "$0")/.." && pwd)
target_root="$project_root/.integration/openfpgaos/src/fpga/targets/pocket"
out_root="$project_root/build/rtl/storage-tx-tap"
mkdir -p "$out_root"
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "$(wslpath -w "$project_root/tests/prepare-storage-tx-tap-sim.ps1")"
verilator --cc --exe --build -j 4 -Wall -Wno-fatal -Wno-BADSTDPRAGMA \
    +define+CHISBURNER_STORAGE_EVENTS +define+CHISBURNER_STORAGE_AUDIT \
    --top-module tb_storage_tx_tap --Mdir "$out_root/model" -CFLAGS '-std=c++17 -O2' \
    "$project_root/tests/rtl/tb_storage_tx_tap.v" "$out_root/io_bridge_peripheral_sim.v" \
    "$target_root/apf/common.v" "$project_root/tests/rtl/tb_storage_tx_tap.cpp" \
    > "$out_root/build.log" 2>&1
"$out_root/model/Vtb_storage_tx_tap" | tee "$out_root/test.log"
