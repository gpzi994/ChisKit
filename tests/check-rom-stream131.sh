#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/stream131
iverilog -g2012 -Wall -s tb_rom_stream131 -o build/stream131/stream.vvp \
 core/rtl/chisburner_rom_stream131.v tests/tb-rom-stream131.v
vvp build/stream131/stream.vvp
iverilog -g2012 -s tb_compare_path131 -o build/stream131/compare-path.vvp \
 core/rtl/chisburner_compare_path131.v core/rtl/chisburner_compare_engine131.v \
 core/rtl/chisburner_psram_stream131.v core/rtl/chisburner_read_cdc131.v \
 core/rtl/chisburner_psram_word_reader131.v tests/tb-compare-path131.v
vvp build/stream131/compare-path.vvp
iverilog -g2012 -Wall -s tb_raw_adapter131 -o build/stream131/raw.vvp \
 core/rtl/chisburner_raw_adapter131.v core/rtl/chisburner_nor_bus108.v tests/tb-raw-adapter131.v
vvp build/stream131/raw.vvp
iverilog -g2012 -Wall -s tb_stream_raw131 -o build/stream131/integration.vvp \
 core/rtl/chisburner_rom_stream131.v core/rtl/chisburner_raw_adapter131.v \
 core/rtl/chisburner_nor_bus108.v tests/tb-stream-raw131.v
vvp build/stream131/integration.vvp
iverilog -g2012 -Wall -s tb_psram_stream131 -o build/stream131/psram.vvp \
 core/rtl/chisburner_read_cdc131.v core/rtl/chisburner_psram_stream131.v \
 core/rtl/chisburner_psram_word_reader131.v tests/tb-psram-stream131.v
vvp build/stream131/psram.vvp
for clocks in '7 5' '5 11'; do
 read -r source_half memory_half <<< "$clocks"
 iverilog -g2012 -s tb_psram_stream131 \
  -Ptb_psram_stream131.SOURCE_HALF="$source_half" \
  -Ptb_psram_stream131.MEMORY_HALF="$memory_half" \
  -o build/stream131/psram-clock.vvp \
  core/rtl/chisburner_read_cdc131.v core/rtl/chisburner_psram_stream131.v \
  core/rtl/chisburner_psram_word_reader131.v tests/tb-psram-stream131.v
 vvp build/stream131/psram-clock.vvp
done
iverilog -g2012 -Wall -s tb_psram_owner131 -o build/stream131/owner.vvp \
 core/rtl/chisburner_psram_owner131.v core/rtl/chisburner_psram_ports131.v \
 tests/tb-psram-owner131.v
vvp build/stream131/owner.vvp
iverilog -g2012 -s tb_cache_lease131 -o build/stream131/lease.vvp \
 core/rtl/chisburner_psram_owner131.v core/rtl/chisburner_cache_lease131.v tests/tb-cache-lease131.v
vvp build/stream131/lease.vvp
iverilog -g2012 -s tb_nor_poll131 -o build/stream131/poll.vvp \
 core/rtl/chisburner_nor_poll131.v tests/tb-nor-poll131.v
vvp build/stream131/poll.vvp
iverilog -g2012 -s tb_nor_buffer131 -o build/stream131/buffer.vvp \
 core/rtl/chisburner_nor_buffer131.v tests/tb-nor-buffer131.v
vvp build/stream131/buffer.vvp
iverilog -g2012 -s tb_nor_program131 -o build/stream131/program.vvp \
 core/rtl/chisburner_nor_program131.v core/rtl/chisburner_nor_buffer131.v \
 core/rtl/chisburner_nor_poll131.v tests/tb-nor-program131.v
vvp build/stream131/program.vvp
iverilog -g2012 -s tb_stream_regs131 -o build/stream131/regs.vvp \
 core/rtl/chisburner_gb_compare207.v \
 core/rtl/chisburner_compare_engine131.v \
 core/rtl/chisburner_stream_regs131.v core/rtl/chisburner_nor_program131.v \
 core/rtl/chisburner_nor_buffer131.v core/rtl/chisburner_nor_poll131.v tests/tb-stream-regs131.v
vvp build/stream131/regs.vvp
iverilog -g2012 -s tb_program_path131 -o build/stream131/path.vvp \
 core/rtl/chisburner_gb_compare207.v \
 core/rtl/chisburner_compare_engine131.v \
 core/rtl/chisburner_program_path131.v core/rtl/chisburner_stream_regs131.v \
 core/rtl/chisburner_nor_program131.v core/rtl/chisburner_nor_buffer131.v \
 core/rtl/chisburner_nor_poll131.v core/rtl/chisburner_raw_adapter131.v \
 core/rtl/chisburner_nor_bus108.v core/rtl/chisburner_psram_stream131.v \
 core/rtl/chisburner_read_cdc131.v core/rtl/chisburner_psram_word_reader131.v tests/tb-program-path131.v
vvp build/stream131/path.vvp
iverilog -g2012 -s tb_psram_arbiter131 -o build/stream131/arbiter.vvp \
 core/rtl/chisburner_psram_arbiter131.v core/rtl/chisburner_psram_owner131.v tests/tb-psram-arbiter131.v
vvp build/stream131/arbiter.vvp
for clocks in '5 7' '7 5' '5 11'; do
 read -r s m <<< "$clocks"
 iverilog -g2012 -s tb_lease_cdc131 -Ptb_lease_cdc131.S="$s" -Ptb_lease_cdc131.M="$m" \
  -o build/stream131/lease-cdc.vvp core/rtl/chisburner_lease_cdc131.v \
  core/rtl/chisburner_psram_owner131.v tests/tb-lease-cdc131.v
 vvp build/stream131/lease-cdc.vvp
 iverilog -g2012 -s tb_shared_lease131 -Ptb_shared_lease131.S="$s" -Ptb_shared_lease131.M="$m" \
  -o build/stream131/shared-lease.vvp core/rtl/chisburner_lease_cdc131.v \
  core/rtl/chisburner_psram_arbiter131.v core/rtl/chisburner_psram_owner131.v tests/tb-shared-lease131.v
 vvp build/stream131/shared-lease.vvp
done
