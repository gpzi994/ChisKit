package require ::quartus::project

if {[llength $argv] != 3} {
    error "usage: quartus_sh -t configure-stream131.tcl <quartus-dir> <firmware-mif> <seed>"
}
set root [file normalize [file join [file dirname [info script]] ..]]
set out [file normalize [lindex $argv 0]]
set mif [file normalize [lindex $argv 1]]
set seed [lindex $argv 2]
cd $out
project_open ap_core
set_global_assignment -name SEED $seed
set_global_assignment -name OPTIMIZATION_MODE "HIGH PERFORMANCE EFFORT"
set_global_assignment -name PHYSICAL_SYNTHESIS_REGISTER_DUPLICATION OFF
set_global_assignment -name IGNORE_MAX_FANOUT_ASSIGNMENTS ON
remove_all_global_assignments -name MIF_FILE
set_global_assignment -name MIF_FILE $mif
set_global_assignment -name VERILOG_MACRO CHISBURNER_STREAM131
set_global_assignment -name VERILOG_MACRO CHISBURNER_PROGRAM131
foreach name {psram_owner131 cache_lease131 psram_arbiter131} {
    set_global_assignment -name VERILOG_FILE [file join $root core rtl chisburner_${name}.v]
}
foreach name {compare_engine131 lease_cdc131 program_path131 stream_regs131 nor_program131 nor_buffer131 nor_poll131 raw_adapter131 psram_stream131 psram_word_reader131 read_cdc131} {
    set_global_assignment -name VERILOG_FILE [file join $root core rtl chisburner_${name}.v]
}
export_assignments
project_close
