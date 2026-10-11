#!/usr/bin/env bash
set -euo pipefail
project_root=$(cd "$(dirname "$0")/.." && pwd)
test_root="$project_root/.integration/openfpgaos/src/fpga/test"
target_root="$test_root/../targets/pocket"
common_root="$test_root/../common"
out_root="$project_root/build/rtl/storage-events-periph"
defines=(+define+CHISBURNER_STORAGE_TRACE +define+CHISBURNER_STORAGE_EVENTS)
cflags='-std=c++17 -O2'
if [[ "${1:-}" == audit ]]; then
    out_root="$project_root/build/rtl/storage-audit-periph"
    defines+=(+define+CHISBURNER_STORAGE_AUDIT)
    cflags+=' -DAUDIT_TEST'
fi
mkdir -p "$out_root"
cd "$test_root"
verilator --cc --exe --build -j 4 -Wall -Wno-fatal -Wno-BADSTDPRAGMA \
    "${defines[@]}" --top-module tb_axi_periph \
    --Mdir "$out_root/model" -CFLAGS "$cflags" -I"$common_root" \
    tb_axi_periph.v altsyncram_stub.v "$common_root/axi_periph_slave.v" \
    "$common_root/sync_fifo.v" "$target_root/apf/common.v" \
    "$project_root/tests/rtl/tb_storage_events_periph.cpp" > "$out_root/build.log" 2>&1
"$out_root/model/Vtb_axi_periph" | tee "$out_root/test.log"
