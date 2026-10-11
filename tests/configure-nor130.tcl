package require ::quartus::project

if {[llength $argv] != 3} {
    error "usage: quartus_sh -t configure-nor130.tcl <quartus-dir> <firmware-mif> <seed>"
}
set out [file normalize [lindex $argv 0]]
set mif [file normalize [lindex $argv 1]]
set seed [lindex $argv 2]
cd $out
project_open ap_core
set_global_assignment -name SEED $seed
remove_all_global_assignments -name MIF_FILE
set_global_assignment -name MIF_FILE $mif
set_global_assignment -name VERILOG_MACRO CHISBURNER_PSRAM_CACHE129
set_global_assignment -name VERILOG_FILE "C:/ONS_FPGA/work/ChisBurner/core/rtl/chisburner_psram_cache129.v"
export_assignments
project_close
