# Isolated full synthesis, fit and STA; no assembler or release packaging.
package require ::quartus::project
package require ::quartus::flow
set root [file normalize [file join [file dirname [info script]] ..]]
set pocket [file join $root .integration openfpgaos src fpga targets pocket]
set baseline [file join $pocket bld chisburner-ui111-s4-floorplan5control-win]
set out [file join $root build sdram121 quartus]
file mkdir $out
cd $out
project_new ap_core -overwrite
source [file join $baseline ap_core.qsf]
file copy -force [file join $baseline firmware.mif] [file join $out firmware.mif]
foreach macro {CHISBURNER_SDRAM118 CHISBURNER_SDRAM120} {
    set_global_assignment -name VERILOG_MACRO $macro
}
foreach name {pocket120 read117 control116 interlock119 ingress113 egress114} {
    set_global_assignment -name VERILOG_FILE [file join $root core rtl chisburner_sdram_${name}.v]
}
set_global_assignment -name SDC_FILE [file join $root core rtl chisburner_sdram121.sdc]
set_instance_assignment -name SYNCHRONIZER_IDENTIFICATION FORCED_IF_ASYNCHRONOUS -to {*|sd120_drained_cpu[*]}
set_instance_assignment -name PRESERVE_REGISTER ON -to {*|sd120_drained_cpu[*]}
export_assignments
execute_module -tool map
execute_module -tool fit
execute_module -tool sta
project_close
