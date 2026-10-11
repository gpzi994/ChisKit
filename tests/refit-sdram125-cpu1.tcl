package require ::quartus::project
package require ::quartus::flow
set root [file normalize [file join [file dirname [info script]] ..]]
cd [file join $root build sdram125-cpu1 quartus]
project_open ap_core
set_global_assignment -name SEED 4
set_global_assignment -name SDC_FILE [file join $root core rtl chisburner_sdram125_cpu.sdc]
export_assignments
execute_module -tool fit
execute_module -tool sta
project_close
