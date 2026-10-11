#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
root="$PWD"
mkdir -p build/sdram122
cd .integration/openfpgaos/src/firmware/os
# Explicit diagnostic fixture IDs only; not a deployable package manifest.
make TARGET=pocket BLD=bld/sdram122-boot \
 EXTRA_CFLAGS='-DCHISBURNER_SDRAM122=1 -DCHISBURNER_SDRAM122_BOOT=1 -DSDRAM122_INPUT_SLOT=120 -DSDRAM122_OUTPUT_SLOT=121 -DCART_UI98=1 -DCART_MMIO_ENABLED=1' \
 bld/sdram122-boot/firmware.elf > "$root/build/sdram122/os-build.log" 2>&1
riscv64-unknown-elf-readelf -SW bld/sdram122-boot/firmware.elf > "$root/build/sdram122/sections.txt"
riscv64-unknown-elf-nm -n bld/sdram122-boot/firmware.elf > "$root/build/sdram122/symbols.txt"
grep -E 'sd122_nocache|os_bss' "$root/build/sdram122/sections.txt"
grep -E 'sd122_buffers|__os_bss_(start|end)|of_file_sdram122_roundtrip' "$root/build/sdram122/symbols.txt"
echo 'SDRAM122 OS compile/link PASS (not a hardware test)'
