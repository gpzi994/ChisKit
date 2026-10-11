#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
root="$PWD"
mkdir -p build/psram127-boot
cd .integration/openfpgaos/src/firmware/os
make TARGET=pocket BLD=bld/psram127-boot-r2 \
 EXTRA_CFLAGS='-DCHISBURNER_PSRAM127=1 -DCHISBURNER_PSRAM127_BOOT=1 -DCART_UI98=1 -DCART_MMIO_ENABLED=1' \
 os.bin bld/psram127-boot-r2/firmware.mif > "$root/build/psram127-boot/os-build.log" 2>&1
riscv64-unknown-elf-readelf -SW bld/psram127-boot-r2/firmware.elf > "$root/build/psram127-boot/sections.txt"
riscv64-unknown-elf-nm -n bld/psram127-boot-r2/firmware.elf > "$root/build/psram127-boot/symbols.txt"
sha256sum bld/psram127-boot-r2/os.bin bld/psram127-boot-r2/firmware.mif
echo 'PSRAM127 direct-SDRAM boot diagnostic built; hardware validation pending.'
