# Development elaboration only; does not produce a release image.
package require ::quartus::project
package require ::quartus::flow
set root [file normalize [file join [file dirname [info script]] ..]]
set baseline [file join $root build nor130-r1-seed6 quartus]
set out [file join $root build stream131 quartus]
file mkdir $out
cd $out
project_new ap_core -overwrite
source [file join $baseline ap_core.qsf]
set_global_assignment -name VERILOG_MACRO CHISBURNER_STREAM131
set_global_assignment -name VERILOG_MACRO CHISBURNER_PROGRAM131
foreach name {psram_owner131 cache_lease131 psram_arbiter131} {
 set_global_assignment -name VERILOG_FILE [file join $root core rtl chisburner_${name}.v]
}
foreach name {compare_engine131 lease_cdc131 program_path131 stream_regs131 nor_program131 nor_buffer131 nor_poll131 raw_adapter131 psram_stream131 read_cdc131} {
 set_global_assignment -name VERILOG_FILE [file join $root core rtl chisburner_${name}.v]
}
export_assignments
execute_module -tool map -args "--analysis_and_elaboration"
project_close
