#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
root=$PWD
target="$root/.integration/openfpgaos/src/fpga/targets/pocket"
test="$root/.integration/openfpgaos/src/fpga/test"
mkdir -p build/sdram120
verilator --cc --exe --build -j 4 -Wno-fatal -Wno-BADSTDPRAGMA \
 +define+CHISBURNER_SDRAM120 --top-module tb_core_bridge_cmd \
 --Mdir "$root/build/sdram120/apf" "$test/tb_core_bridge_cmd.v" \
 "$test/altsyncram_stub.v" "$target/core_bridge_cmd.v" \
 "$target/apf/mf_datatable.v" "$target/apf/common.v" \
 "$root/tests/rtl/tb_sdram120_apf.cpp" >build/sdram120/apf-build.log 2>&1
build/sdram120/apf/Vtb_core_bridge_cmd
