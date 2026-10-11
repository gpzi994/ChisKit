#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/sdram115
for variant in ingress113:4 ingress113:10 egress114:0; do
 kind=${variant%:*}
 bits=${variant#*:}
 tag="$kind-$bits"
 extra=()
 if [[ "$kind" == ingress113 ]]; then
  extra=(-GFIFO_BITS="$bits" -CFLAGS "-DFIFO_WORDS=$((1<<bits))")
 fi
 # Existing arbiter compares a 4-bit queue count to 5'd0. Keep all other
 # warnings fatal, including missing/miswired integration ports.
 verilator --cc --exe --build -j 4 -Wno-WIDTHEXPAND \
  --top-module "sdram115_$kind" --prefix "Vchisburner_sdram_$kind" \
  "${extra[@]}" --Mdir "build/sdram115/$tag" \
  "tests/sdram115_$kind.v" "core/rtl/chisburner_sdram_$kind.v" \
  .integration/openfpgaos/src/fpga/common/axi_sdram_arbiter.v \
  "$PWD/tests/test-sdram${kind: -3}-${kind:0: -3}.cpp" \
  >"build/sdram115/$tag-build.log" 2>&1
 "build/sdram115/$tag/Vchisburner_sdram_$kind"
done
echo 'PASS SDRAM115: real arbiter M2 integration, other masters idle; not hardware validation.'
