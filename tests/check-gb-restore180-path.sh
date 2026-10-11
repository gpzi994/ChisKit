#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/gb-restore180-tests
iverilog -g2012 ${EXTRA_RTL_FLAGS:-} -DCHISBURNER_GB_RESTORE180 -DCHISBURNER_GB_SAVE179 -DCHISBURNER_GB_SAVE177 -DCHISBURNER_GB_SAVE175 -DCHISBURNER_EEPROM139 \
 -DCHISBURNER_SAVE139 -DCHISBURNER_SAVE158 -DCHISBURNER_SRAM137 -DCHISBURNER_SRAM138 \
 -s tb_gb_save175_path -o build/gb-restore180-tests/path.vvp \
 core/rtl/chisburner_gb_compare207.v \
 core/rtl/chisburner_program_path131.v core/rtl/chisburner_stream_regs131.v \
 core/rtl/chisburner_compare_engine131.v core/rtl/chisburner_nor_program131.v \
 core/rtl/chisburner_nor_buffer131.v core/rtl/chisburner_nor_poll131.v \
 core/rtl/chisburner_raw_adapter131.v core/rtl/chisburner_nor_bus108.v \
 core/rtl/chisburner_psram_stream131.v core/rtl/chisburner_read_cdc131.v \
 core/rtl/chisburner_psram_word_reader131.v core/rtl/chisburner_eeprom_path139.v \
 core/rtl/chisburner_eeprom_regs139.v core/rtl/chisburner_eeprom139.v \
 core/rtl/chisburner_eeprom_pins139.v tests/tb-gb-save175-path.v
vvp build/gb-restore180-tests/path.vvp
