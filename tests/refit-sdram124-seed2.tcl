package require ::quartus::project
package require ::quartus::flow
set root [file normalize [file join [file dirname [info script]] ..]]
cd [file join $root build sdram124-seed2 quartus]
project_open ap_core
set_global_assignment -name SEED 2
export_assignments
execute_module -tool fit
execute_module -tool sta
project_close
