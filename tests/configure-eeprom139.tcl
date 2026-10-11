package require ::quartus::project
set out [file normalize [lindex $argv 0]]
set root [file normalize [file join [file dirname [info script]] ..]]
cd $out
project_open ap_core
remove_all_global_assignments -name MIF_FILE
set_global_assignment -name MIF_FILE [file join $out firmware.mif]
set_global_assignment -name VERILOG_MACRO CHISBURNER_EEPROM139
if {[llength $argv]>1 && [lindex $argv 1] eq "auto-save"} {
 set_global_assignment -name VERILOG_MACRO CHISBURNER_SAVE139
}
foreach name {eeprom139 eeprom_regs139 eeprom_pins139 eeprom_path139} {
 set_global_assignment -name VERILOG_FILE [file join $root core rtl chisburner_${name}.v]
}
export_assignments
project_close
