#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
root="$PWD"
mkdir -p build/sram137
cd .integration/openfpgaos/src/firmware/os
make -j4 TARGET=pocket BLD=bld/sram137-release \
 EXTRA_CFLAGS='-DCART_MMIO_ENABLED=1 -DCART_UI98=1 -DCART_NOR108=1 -DCART_NOR109=1 -DCART_NOR110=1 -DCART_STREAM131=1 -DCART_SRAM137=1 -DCART_S29GL256_PROGRAMMER=1 -DCHISBURNER_OUTPUT_ENABLED=1 -DCHISBURNER_PSRAM127=1 -DCHISBURNER_PSRAM_CACHE129=1' \
 os.bin bld/sram137-release/firmware.mif > "$root/build/sram137/os-build.log" 2>&1
sha256sum bld/sram137-release/os.bin bld/sram137-release/firmware.mif
