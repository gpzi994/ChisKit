package require ::quartus::project
if {[llength $argv] != 2} {
 error "usage: quartus_sh -t configure-eeprom162-seed.tcl <quartus-dir> <seed>"
}
set out [file normalize [lindex $argv 0]]
set seed [lindex $argv 1]
if {![string is integer -strict $seed] || $seed < 1 || $seed > 100} {
 error "invalid fitter seed"
}
cd $out
project_open ap_core
remove_all_global_assignments -name MIF_FILE
set_global_assignment -name MIF_FILE [file join $out firmware.mif]
set_global_assignment -name VERILOG_MACRO CHISBURNER_SAVE158
set_global_assignment -name SEED $seed
export_assignments
project_close
