#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
root="$PWD"
mkdir -p build/nor130
cd .integration/openfpgaos/src/firmware/os
make TARGET=pocket BLD=bld/nor130 \
 EXTRA_CFLAGS='-DCART_MMIO_ENABLED=1 -DCART_UI98=1 -DCART_NOR108=1 -DCART_NOR109=1 -DCART_NOR110=1 -DS29GL256_PROGRAMMER=1 -DCHISBURNER_OUTPUT_ENABLED=1 -DCHISBURNER_PSRAM127=1 -DCHISBURNER_PSRAM_CACHE129=1' \
 os.bin bld/nor130/firmware.mif > "$root/build/nor130/os-build.log" 2>&1
riscv64-unknown-elf-readelf -SW bld/nor130/firmware.elf > "$root/build/nor130/sections.txt"
riscv64-unknown-elf-nm -n bld/nor130/firmware.elf > "$root/build/nor130/symbols.txt"
riscv64-unknown-elf-objdump -dr bld/nor130/firmware.elf > "$root/build/nor130/disassembly.txt"
sha256sum bld/nor130/os.bin bld/nor130/firmware.mif
echo 'NOR130 production OS built.'
