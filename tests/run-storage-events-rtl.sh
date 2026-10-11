#!/usr/bin/env bash
set -euo pipefail
root=$(cd "$(dirname "$0")/.." && pwd)
out="$root/build/rtl/storage-events"
mkdir -p "$out"
verilator --cc --exe --build -j 4 -Wall -Wno-fatal \
    --top-module pct_bridge_trace --Mdir "$out/model" \
    "$root/core/storage/pct_bridge_trace.v" "$root/tests/rtl/tb_storage_events.cpp" \
    > "$out/build.log" 2>&1
"$out/model/Vpct_bridge_trace" | tee "$out/test.log"
