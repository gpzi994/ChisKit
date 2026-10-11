#!/usr/bin/env bash
set -euo pipefail
project_root=$(cd "$(dirname "$0")/.." && pwd)
out_root="$project_root/build/rtl/storage-audit"
mkdir -p "$out_root"
verilator --cc --exe --build -j 4 -Wall -Wno-fatal --top-module pct_bridge_audit \
    --Mdir "$out_root/model" -CFLAGS '-std=c++17 -O2' \
    "$project_root/core/storage/pct_bridge_audit.v" "$project_root/tests/rtl/tb_storage_audit.cpp" \
    > "$out_root/build.log" 2>&1
"$out_root/model/Vpct_bridge_audit" | tee "$out_root/test.log"
