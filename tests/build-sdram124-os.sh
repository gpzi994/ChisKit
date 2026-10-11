#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
root="$PWD"
mkdir -p build/sdram124
gcc -std=c11 -Wall -Wextra -Werror tests/make-sdram123-fixture.c -o build/sdram124/make-fixture
build/sdram124/make-fixture build/sdram124/fixture123.bin
cd .integration/openfpgaos/src/firmware/os
make TARGET=pocket BLD=bld/sdram124-boot \
 EXTRA_CFLAGS='-DCHISBURNER_SDRAM124=1 -DCHISBURNER_SDRAM122=1 -DCHISBURNER_SDRAM122_BOOT=1 -DCHISBURNER_SDRAM123_FIXTURE=1 -DSDRAM122_INPUT_SLOT=4 -DSDRAM122_OUTPUT_SLOT=5 -DCART_UI98=1 -DCART_MMIO_ENABLED=1' \
 os.bin bld/sdram124-boot/firmware.mif > "$root/build/sdram124/os-build.log" 2>&1
riscv64-unknown-elf-readelf -SW bld/sdram124-boot/firmware.elf > "$root/build/sdram124/sections.txt"
riscv64-unknown-elf-nm -n bld/sdram124-boot/firmware.elf > "$root/build/sdram124/symbols.txt"
sha256sum bld/sdram124-boot/os.bin "$root/build/sdram124/fixture123.bin"
echo 'SDRAM123 OS and deterministic fixture built; hardware validation pending.'
