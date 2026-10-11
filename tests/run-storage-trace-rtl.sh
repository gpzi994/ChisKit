#!/usr/bin/env bash
set -euo pipefail
project_root=$(cd "$(dirname "$0")/.." && pwd)
test_root="$project_root/.integration/openfpgaos/src/fpga/test"
target_root="$test_root/../targets/pocket"
common_root="$test_root/../common"
out_root="$project_root/build/rtl/storage-trace"
mkdir -p "$out_root"
cd "$test_root"
verilator --cc --exe --build -j 4 -Wall -Wno-fatal -Wno-BADSTDPRAGMA \
    +define+CHISBURNER_STORAGE_TRACE --top-module tb_core_bridge_cmd \
    --Mdir "$out_root/bridge" -CFLAGS '-std=c++17 -O2' \
    tb_core_bridge_cmd.v altsyncram_stub.v "$target_root/core_bridge_cmd.v" \
    "$target_root/apf/mf_datatable.v" "$target_root/apf/common.v" \
    "$project_root/tests/rtl/tb_storage_trace_bridge.cpp" > "$out_root/bridge-build.log" 2>&1
"$out_root/bridge/Vtb_core_bridge_cmd" | tee "$out_root/bridge-test.log"
verilator --cc --exe --build -j 4 -Wall -Wno-fatal -Wno-BADSTDPRAGMA \
    +define+CHISBURNER_STORAGE_TRACE --top-module tb_axi_periph \
    --Mdir "$out_root/periph" -CFLAGS '-std=c++17 -O2' -I"$common_root" \
    tb_axi_periph.v altsyncram_stub.v "$common_root/axi_periph_slave.v" \
    "$common_root/sync_fifo.v" "$target_root/apf/common.v" \
    "$project_root/tests/rtl/tb_storage_trace_periph.cpp" > "$out_root/periph-build.log" 2>&1
"$out_root/periph/Vtb_axi_periph" | tee "$out_root/periph-test.log"
