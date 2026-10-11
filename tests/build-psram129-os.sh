#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
root="$PWD"
mkdir -p build/psram129
cd .integration/openfpgaos/src/firmware/os
make TARGET=pocket BLD=bld/psram129 \
 EXTRA_CFLAGS='-DCHISBURNER_PSRAM127=1 -DCHISBURNER_PSRAM129_BOOT=1 -DCHISBURNER_PSRAM_CACHE129=1 -DCART_UI98=1 -DCART_MMIO_ENABLED=1' \
 os.bin bld/psram129/firmware.mif > "$root/build/psram129/os-build.log" 2>&1
riscv64-unknown-elf-readelf -SW bld/psram129/firmware.elf > "$root/build/psram129/sections.txt"
riscv64-unknown-elf-nm -n bld/psram129/firmware.elf > "$root/build/psram129/symbols.txt"
riscv64-unknown-elf-objdump -dr bld/psram129/firmware.elf > "$root/build/psram129/disassembly.txt"
sha256sum bld/psram129/os.bin bld/psram129/firmware.mif
echo 'PSRAM129 dual-PSRAM cache diagnostic OS built.'
