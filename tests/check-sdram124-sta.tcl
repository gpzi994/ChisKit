# Run after check-sdram121-fit.tcl. Detailed CDC evidence, not a PASS stamp.
set root [file normalize [file join [file dirname [info script]] ..]]
if {$argc == 1} {cd [lindex $argv 0]} else {cd [file join $root build sdram124 quartus]}
project_open ap_core
create_timing_netlist
read_sdc
update_timing_netlist
report_net_delay -nworst 200 -file cdc-net-delay.txt
report_metastability -nchains 1000 -file cdc-metastability.txt
report_timing -setup -npaths 20 -detail full_path -file setup-paths.txt
report_timing -hold -npaths 20 -detail full_path -file hold-paths.txt
report_ucp -file unconstrained.txt
set corner 0
foreach_in_collection op [get_available_operating_conditions] {
    set_operating_conditions $op
    update_timing_netlist
    puts "SD121 corner $corner: $op"
    report_net_delay -nworst 200 -file cdc-net-delay-$corner.txt
    report_timing -hold -npaths 5 -detail full_path -file hold-corner-$corner.txt
    incr corner
}
delete_timing_netlist
project_close
