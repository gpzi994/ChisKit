package require ::quartus::project
package require ::quartus::flow
set root [file normalize [file join [file dirname [info script]] ..]]
cd [file join $root build sdram125-region1 quartus]
project_open ap_core
set_global_assignment -name SEED 4
# In the best unconstrained fit the FpuF2i launch register was X13_Y29 while
# both SRC1[17] destinations landed near X2_Y18. Keep only those destination
# copies in the central CPU area; do not constrain the surrounding cone.
set_instance_assignment -name PLACE_REGION "X8 Y14 X18 Y34" -to {*|execute_ctrl2_up_early0_SrcPlugin_SRC1_lane0[17]*}
export_assignments
execute_module -tool fit
execute_module -tool sta
project_close
