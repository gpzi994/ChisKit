#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/gb-compare208-tests
for clocks in '5 7' '7 5'; do
 read -r s m <<< "$clocks"
 iverilog -g2012 -s tb_gb_compare208 -Ptb_gb_compare208.S="$s" -Ptb_gb_compare208.M="$m" \
 -o build/gb-compare208-tests/path.vvp \
 core/rtl/chisburner_gb_compare207.v core/rtl/chisburner_compare_engine131.v \
 core/rtl/chisburner_program_path131.v core/rtl/chisburner_stream_regs131.v \
 core/rtl/chisburner_nor_program131.v core/rtl/chisburner_nor_buffer131.v \
 core/rtl/chisburner_nor_poll131.v core/rtl/chisburner_raw_adapter131.v \
 core/rtl/chisburner_nor_bus108.v core/rtl/chisburner_psram_stream131.v \
 core/rtl/chisburner_read_cdc131.v core/rtl/chisburner_psram_word_reader131.v tests/tb-gb-compare208.v
 vvp build/gb-compare208-tests/path.vvp
done
