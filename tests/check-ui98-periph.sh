#!/usr/bin/env bash
set -euo pipefail
root=$(cd "$(dirname "$0")/.." && pwd)
cd "$root"
rtl=.integration/openfpgaos/src/fpga/targets/pocket
common=.integration/openfpgaos/src/fpga/common
extra=()
tag=ui98-periph
if [[ "${SDRAM118:-0}" == 1 ]]; then
 extra=(+define+CHISBURNER_SDRAM118 -CFLAGS -DTEST_SDRAM118 core/rtl/chisburner_sdram_control116.v)
 tag=sdram118-periph
fi
mkdir -p build
if [[ "${EEPROM139:-0}" == 1 ]]; then
 extra+=(+define+CHISBURNER_EEPROM139 -CFLAGS -DTEST_EEPROM139)
 tag=eeprom139-periph
fi
if [[ "${PROGRAM131:-0}" == 1 ]]; then
 extra+=(+define+CHISBURNER_PROGRAM131 -CFLAGS -DTEST_PROGRAM131)
 tag=stream131-periph
fi
if [[ "${GPU_LEAN132:-0}" == 1 ]]; then
 extra+=(+define+EXCLUDE_GPU -CFLAGS -DTEST_GPU_LEAN132)
 tag=gpu-lean132-periph
fi
if [[ "${GBA_IDENTIFY133:-0}" == 1 ]]; then
 extra+=(+define+CHISBURNER_GBA_IDENTIFY133 -CFLAGS -DTEST_GBA_IDENTIFY133)
 tag=gba-identify133-periph
fi
if [[ "${GB_IDENTIFY134:-0}" == 1 ]]; then
 extra+=(+define+CHISBURNER_GB_IDENTIFY134 -CFLAGS -DTEST_GB_IDENTIFY134)
 tag=gb-identify134-periph
fi
if [[ "${TIMERS135:-0}" == 1 ]]; then
 extra+=(+define+CHISBURNER_TIMERS135)
 tag=timers135-periph
fi
verilator --cc --exe --build -j 4 -Wno-fatal -Wno-BADSTDPRAGMA \
 +define+CHISBURNER_STORAGE_TRACE +define+CHISBURNER_STORAGE_EVENTS \
 +define+CHISBURNER_STORAGE_AUDIT +define+CHISBURNER_GBA_PROGRAM65 \
 --top-module tb_axi_periph -GINCLUDE_CARTRIDGE=1 --Mdir "build/$tag" -I"$common" "${extra[@]}" \
 tests/rtl/tb_axi_periph_ui98.v .integration/openfpgaos/src/fpga/test/altsyncram_stub.v \
 "$common/axi_periph_slave.v" "$common/sync_fifo.v" "$rtl/apf/common.v" \
 core/rtl/chisburner_cart_auto98.v "$rtl/chisburner_cart_gb_readonly.v" \
 "$rtl/chisburner_cart_gb_readonly_abstract.v" "$rtl/chisburner_cart_gb_mbc5_fullrom.v" \
 "$rtl/chisburner_cart_gb_mbc5_identify.v" "$rtl/chisburner_cart_gba_readonly.v" \
 "$rtl/chisburner_cart_gba_s29gl256.v" "$root/tests/rtl/tb_ui98_periph.cpp" >"build/$tag.log" 2>&1
"build/$tag/Vtb_axi_periph"
