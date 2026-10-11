# Development-only full Pocket elaboration. Never assembles a release bitstream.
package require ::quartus::project
package require ::quartus::flow
set root [file normalize [file join [file dirname [info script]] ..]]
set pocket [file join $root .integration openfpgaos src fpga targets pocket]
set baseline [file join $pocket bld chisburner-ui111-s4-floorplan5control-win]
set out [file join $root build sdram120 quartus]
file mkdir $out
cd $out
project_new ap_core -overwrite
source [file join $baseline ap_core.qsf]
file copy -force [file join $baseline firmware.mif] [file join $out firmware.mif]
set_global_assignment -name VERILOG_MACRO CHISBURNER_SDRAM118
set_global_assignment -name VERILOG_MACRO CHISBURNER_SDRAM120
foreach name {pocket120 read117 control116 interlock119 ingress113 egress114} {
    set_global_assignment -name VERILOG_FILE [file join $root core rtl chisburner_sdram_${name}.v]
}
export_assignments
execute_module -tool map -args "--analysis_and_elaboration"
project_close
