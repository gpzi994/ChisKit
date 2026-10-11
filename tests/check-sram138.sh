#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/sram138
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined \
 core/save/hardware138.c tests/test-save-hardware138.c -o build/sram138/evidence-test
build/sram138/evidence-test
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined \
 -I.integration/openfpgaos/src/firmware/api tests/test-sram138-transfer.c \
 -o build/sram138/transfer-test
build/sram138/transfer-test
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined \
 -DCB_SRAM137 -DCB_SRAM138 -I.integration/openfpgaos/src/firmware/api \
 tests/test-sram138-ui.c app/ui98/ui.c app/ui98/font.c app/ui98/model.c \
 -o build/sram138/ui-test
build/sram138/ui-test
gcc -std=c11 -D_GNU_SOURCE -Wall -Wextra -Werror -g -fsanitize=address,undefined \
 -Icore/nor -I.integration/openfpgaos/src/firmware/api \
 tests/test-sram138-os.c core/nor/recipe.c \
 core/nor/mori/chisburner_nor_db_data.c core/nor/mori/chisburner_nor_db_lookup.c \
 -o build/sram138/os-test
build/sram138/os-test
verilator --cc --exe --build -j 4 -Wno-fatal \
 +define+CHISBURNER_TIMERS135 +define+CHISBURNER_SRAM137 +define+CHISBURNER_SRAM138 \
 -CFLAGS '-DTEST_SRAM137 -DTEST_SRAM138' \
 --top-module chisburner_nor_bus108 --Mdir build/sram138/bus \
 core/rtl/chisburner_nor_bus108.v "$PWD/tests/test-nor108-bus.cpp" \
 >build/sram138/bus-build.log 2>&1
build/sram138/bus/Vchisburner_nor_bus108
