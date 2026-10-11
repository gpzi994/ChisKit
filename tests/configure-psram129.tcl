package require ::quartus::project

if {[llength $argv] != 2} {
    error "usage: quartus_sh -t configure-psram129.tcl <quartus-dir> <seed>"
}
set out [file normalize [lindex $argv 0]]
set seed [lindex $argv 1]
cd $out
project_open ap_core
set_global_assignment -name SEED $seed
set_global_assignment -name VERILOG_MACRO CHISBURNER_PSRAM_CACHE129
set_global_assignment -name VERILOG_FILE "C:/ONS_FPGA/work/ChisBurner/core/rtl/chisburner_psram_cache129.v"
export_assignments
project_close
