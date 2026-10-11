#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
root="$PWD"
mkdir -p build/psram128
cd .integration/openfpgaos/src/firmware/os
make TARGET=pocket BLD=bld/psram128 \
 EXTRA_CFLAGS='-DCHISBURNER_PSRAM127=1 -DCHISBURNER_PSRAM128_BOOT=1 -DCART_UI98=1 -DCART_MMIO_ENABLED=1' \
 os.bin bld/psram128/firmware.mif > "$root/build/psram128/os-build.log" 2>&1
riscv64-unknown-elf-readelf -SW bld/psram128/firmware.elf > "$root/build/psram128/sections.txt"
riscv64-unknown-elf-nm -n bld/psram128/firmware.elf > "$root/build/psram128/symbols.txt"
riscv64-unknown-elf-objdump -dr bld/psram128/firmware.elf > "$root/build/psram128/disassembly.txt"
sha256sum bld/psram128/os.bin bld/psram128/firmware.mif
echo 'PSRAM128 SDRAM-only file/save diagnostic built; hardware validation pending.'
