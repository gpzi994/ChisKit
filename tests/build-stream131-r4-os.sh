#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
root="$PWD"
release="${STREAM131_RELEASE:-stream131-r4-release}"
mkdir -p "build/$release"
cd .integration/openfpgaos/src/firmware/os
make TARGET=pocket BLD="bld/$release" \
 EXTRA_CFLAGS='-DCART_MMIO_ENABLED=1 -DCART_UI98=1 -DCART_NOR108=1 -DCART_NOR109=1 -DCART_NOR110=1 -DCART_STREAM131=1 -DCART_S29GL256_PROGRAMMER=1 -DCHISBURNER_OUTPUT_ENABLED=1 -DCHISBURNER_PSRAM127=1 -DCHISBURNER_PSRAM_CACHE129=1' \
 os.bin "bld/$release/firmware.mif" > "$root/build/$release/os-build.log" 2>&1
sha256sum "bld/$release/os.bin" "bld/$release/firmware.mif"
