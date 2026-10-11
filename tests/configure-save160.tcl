package require ::quartus::project
if {[llength $argv] != 1} {
    error "usage: quartus_sh -t configure-save160.tcl <quartus-dir>"
}
set out [file normalize [lindex $argv 0]]
cd $out
project_open ap_core
remove_all_global_assignments -name MIF_FILE
set_global_assignment -name MIF_FILE [file join $out firmware.mif]
set_global_assignment -name VERILOG_MACRO CHISBURNER_SAVE158
export_assignments
project_close
