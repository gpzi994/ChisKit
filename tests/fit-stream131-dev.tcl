# Isolated Stream131 full synthesis, fit and STA. No release package is made.
package require ::quartus::project
package require ::quartus::flow
set root [file normalize [file join [file dirname [info script]] ..]]
set baseline [file join $root build nor130-r1-seed6 quartus]
set out [file join $root build stream131 fit-seed6-lifecycle]
file mkdir $out
cd $out
project_new ap_core -overwrite
source [file join $baseline ap_core.qsf]
file copy -force [file join $root .integration openfpgaos src firmware os bld stream131-dev firmware.mif] [file join $out firmware.mif]
set_global_assignment -name MIF_FILE [file join $out firmware.mif]
set_global_assignment -name VERILOG_MACRO CHISBURNER_STREAM131
set_global_assignment -name VERILOG_MACRO CHISBURNER_PROGRAM131
foreach name {psram_owner131 cache_lease131 psram_arbiter131} {
 set_global_assignment -name VERILOG_FILE [file join $root core rtl chisburner_${name}.v]
}
foreach name {compare_engine131 lease_cdc131 program_path131 stream_regs131 nor_program131 nor_buffer131 nor_poll131 raw_adapter131 psram_stream131 psram_word_reader131 read_cdc131} {
 set_global_assignment -name VERILOG_FILE [file join $root core rtl chisburner_${name}.v]
}
set_global_assignment -name SEED 6
set_global_assignment -name OPTIMIZATION_MODE "HIGH PERFORMANCE EFFORT"
set_global_assignment -name PHYSICAL_SYNTHESIS_REGISTER_DUPLICATION OFF
# The ChisBurner CPU image carries a maxfan=16 performance hint which inserts
# 179 duplicate logic cells. Stream131 is LAB-bound; validate the unrouted net
# directly with full STA instead of reserving those cells unconditionally.
set_global_assignment -name IGNORE_MAX_FANOUT_ASSIGNMENTS ON
export_assignments
execute_module -tool map
execute_module -tool fit
execute_module -tool sta
project_close
