#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
root="$PWD"
mkdir -p build/save139
cd .integration/openfpgaos/src/firmware/os
# Integration-only build with the optional detection service;
# do not deploy this OS with a release FPGA that lacks the EEPROM capability.
# os.bin must pass the reviewed additive register contract gate.
make -j4 TARGET=pocket BLD=bld/eeprom139-integration \
 EXTRA_CFLAGS='-DCART_MMIO_ENABLED=1 -DCART_UI98=1 -DCART_NOR108=1 -DCART_NOR109=1 -DCART_NOR110=1 -DCART_STREAM131=1 -DCART_SRAM137=1 -DCART_SRAM138=1 -DCART_EEPROM139=1 -DCART_S29GL256_PROGRAMMER=1 -DCHISBURNER_OUTPUT_ENABLED=1 -DCHISBURNER_PSRAM127=1 -DCHISBURNER_PSRAM_CACHE129=1' \
 os.bin bld/eeprom139-integration/firmware.mif > "$root/build/save139/os-build.log" 2>&1
tool=riscv64-unknown-elf-nm
command -v "$tool" >/dev/null || tool=riscv64-elf-nm
"$tool" -S bld/eeprom139-integration/firmware.elf | awk '
 / eeprom139_(small|large)(\.lto_priv\.[0-9]+)?$/ {
  print; n++;
  if ($1 !~ /^103[2-9a-d]/ || $3 !~ /^[bB]$/) bad=1;
  if ($4 ~ /^eeprom139_small/ && $2!="00000200") bad=1;
  if ($4 ~ /^eeprom139_large/ && $2!="00002000") bad=1;
 }
 END {if(n!=2 || bad)exit 1;}'
sha256sum bld/eeprom139-integration/firmware.elf
"$tool" -S bld/eeprom139-integration/firmware.elf | awk '
 / save_auto139_parallel(\.lto_priv\.[0-9]+)?$/ {
  print; n++;
  if ($1 !~ /^103[2-9a-d]/ || $3 !~ /^[bB]$/ || $2!="00020000") bad=1;
 }
 END {if(n!=1 || bad)exit 1;}'
sha256sum bld/eeprom139-integration/os.bin bld/eeprom139-integration/firmware.mif
