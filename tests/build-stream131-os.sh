#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
root="$PWD"
mkdir -p build/stream131-r2-release
cd .integration/openfpgaos/src/firmware/os
make TARGET=pocket BLD=bld/stream131-r2-release \
 EXTRA_CFLAGS='-DCART_MMIO_ENABLED=1 -DCART_UI98=1 -DCART_NOR108=1 -DCART_NOR109=1 -DCART_NOR110=1 -DCART_STREAM131=1 -DCART_S29GL256_PROGRAMMER=1 -DCHISBURNER_OUTPUT_ENABLED=1 -DCHISBURNER_PSRAM127=1 -DCHISBURNER_PSRAM_CACHE129=1' \
 os.bin bld/stream131-r2-release/firmware.mif > "$root/build/stream131-r2-release/os-build.log" 2>&1
riscv64-unknown-elf-readelf -SW bld/stream131-r2-release/firmware.elf > "$root/build/stream131-r2-release/sections.txt"
riscv64-unknown-elf-nm -n bld/stream131-r2-release/firmware.elf > "$root/build/stream131-r2-release/symbols.txt"
riscv64-unknown-elf-objdump -dr bld/stream131-r2-release/firmware.elf > "$root/build/stream131-r2-release/disassembly.txt"
sha256sum bld/stream131-r2-release/os.bin bld/stream131-r2-release/firmware.mif
echo 'Stream131 release OS built.'
