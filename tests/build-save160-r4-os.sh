#!/usr/bin/env bash
set -euo pipefail
revision=${1:-r4}
cd "$(dirname "$0")/../.integration/openfpgaos/src/firmware/os"
make -j4 TARGET=pocket BLD=bld/save160-$revision \
 EXTRA_CFLAGS='-DCART_MMIO_ENABLED=1 -DCART_UI98=1 -DCART_NOR108=1 -DCART_NOR109=1 -DCART_NOR110=1 -DCART_STREAM131=1 -DCART_SRAM137=1 -DCART_SRAM138=1 -DCART_EEPROM139=1 -DCART_SAVE158=1 -DCART_S29GL256_PROGRAMMER=1 -DCHISBURNER_OUTPUT_ENABLED=1 -DCHISBURNER_PSRAM127=1 -DCHISBURNER_PSRAM_CACHE129=1' \
 bld/save160-$revision/os.bin bld/save160-$revision/firmware.mif
