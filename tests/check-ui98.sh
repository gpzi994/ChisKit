#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
fixture=${1:?Supply local GBA header fixture}
bash tests/check-ui98-host.sh "$fixture"
bash tests/check-ui98-rtl.sh
bash tests/check-ui98-periph.sh
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tests/check-ui98-output.ps1
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -g \
 -I.integration/openfpgaos/src/firmware/api app/gba_program65/test_program65.c \
 -o build/ui101-program-host
./build/ui101-program-host "$fixture"
bash tests/check-ui97.sh "$fixture"
make -C app/ui98 -j4
rtl=.integration/openfpgaos/src/fpga/targets/pocket
sha256sum app/ui98/*.[ch] app/ui97/model.c app/ui97/font.c \
 app/gba_program65/program65.h app/gba_dump53/md5.h app/gba_db55/sha1_55.h \
 app/gba_program65/test_program65.c \
 app/gba_dump57/reference.h app/storage_guarded51/guard.h app/storage_stream52/gate.h \
 core/rtl/chisburner_cart_auto98.v "$rtl/core_top.v" \
 "$rtl/chisburner_cart_gba_readonly.v" "$rtl/chisburner_cart_gba_s29gl256.v" \
 "$rtl/chisburner_cart_gb_readonly.v" "$rtl/chisburner_cart_gb_readonly_abstract.v" \
 "$rtl/chisburner_cart_gb_mbc5_fullrom.v" "$rtl/chisburner_cart_gb_mbc5_identify.v" \
 .integration/openfpgaos/src/firmware/os/kernel/services_table.c \
 .integration/openfpgaos/src/firmware/os/targets/pocket/file.c \
 tests/test-ui98*.c tests/test-ui98*rtl.cpp tests/rtl/tb_ui98_periph.cpp \
 tests/rtl/tb_axi_periph_ui98.v >build/ui101-tested-sources.sha256
echo 'UI98: all host/RTL/AXI/baseline checks PASS; hardware acceptance pending'
