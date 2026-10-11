package require ::quartus::project
package require ::quartus::atoms

set root [file normalize [file join [file dirname [info script]] ..]]
set build_name [expr {[llength $argv] ? [lindex $argv 0] : "sdram125-cpu1"}]
set out [file join $root build $build_name quartus]
cd $out
project_open ap_core
read_atom_netlist

set fp [open [file join $out atom-ff-locations.txt] w]
foreach_in_collection atom [get_atom_nodes -type FF] {
    set name [get_atom_node_info -node $atom -key NAME]
    set location [get_atom_node_info -node $atom -key LOCATION]
    puts $fp "$location\t$name"
}
close $fp

set fp [open [file join $out atom-comb-locations.txt] w]
foreach_in_collection atom [get_atom_nodes -type COMB] {
    set name [get_atom_node_info -node $atom -key NAME]
    set location [get_atom_node_info -node $atom -key LOCATION]
    puts $fp "$location\t$name"
}
close $fp
project_close
