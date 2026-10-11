package require ::quartus::project
set out [file normalize [lindex $argv 0]]
cd $out
project_open ap_core
remove_all_global_assignments -name MIF_FILE
set_global_assignment -name MIF_FILE [file join $out firmware.mif]
set_global_assignment -name VERILOG_MACRO EXCLUDE_GPU
export_assignments
project_close
