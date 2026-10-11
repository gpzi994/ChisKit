#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/save139
files=()
for n in compare_engine131 program_path131 stream_regs131 nor_program131 nor_buffer131 nor_poll131 raw_adapter131 nor_bus108 psram_stream131 read_cdc131 psram_word_reader131 eeprom139 eeprom_pins139 eeprom_regs139 eeprom_path139; do
 files+=("core/rtl/chisburner_${n}.v")
done
for enabled in 0 1 2; do
 flags=()
 if [ "$enabled" = 1 ]; then flags=(-DCHISBURNER_EEPROM139); fi
 if [ "$enabled" = 2 ]; then
  flags=(-DCHISBURNER_EEPROM139 -DCHISBURNER_SRAM137 -DCHISBURNER_SRAM138 -DCHISBURNER_SAVE139 -DCHISBURNER_TIMERS135)
 fi
 iverilog -g2012 "${flags[@]}" -s tb_program_path131 \
  -o "build/save139/production-$enabled.vvp" "${files[@]}" tests/tb-program-path131.v
 vvp "build/save139/production-$enabled.vvp"
done
