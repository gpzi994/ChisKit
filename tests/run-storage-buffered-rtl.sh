#!/usr/bin/env bash
set -euo pipefail
project_root=$(cd "$(dirname "$0")/.." && pwd)
test_root="$project_root/.integration/openfpgaos/src/fpga/test"
target_root="$test_root/../targets/pocket"
out_root="$project_root/build/rtl/storage-buffered"
mkdir -p "$out_root"
cd "$test_root"
verilator --cc --exe --build -j 4 -Wall -Wno-fatal -Wno-BADSTDPRAGMA \
    +define+CHISBURNER_STORAGE_TRACE --top-module tb_storage_buffered \
    --Mdir "$out_root/model" -CFLAGS '-std=c++17 -O2' \
    "$project_root/tests/rtl/tb_storage_buffered.v" \
    tb_core_bridge_cmd.v altsyncram_stub.v "$target_root/core_bridge_cmd.v" \
    "$target_root/apf/mf_datatable.v" "$target_root/apf/common.v" \
    "$project_root/tests/rtl/tb_storage_buffered.cpp" > "$out_root/build.log" 2>&1
"$out_root/model/Vtb_storage_buffered" | tee "$out_root/test.log"
