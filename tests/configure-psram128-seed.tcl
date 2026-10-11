package require ::quartus::project

if {[llength $argv] != 2} {
    error "usage: quartus_sh -t configure-psram128-seed.tcl <quartus-dir> <seed>"
}

set out [file normalize [lindex $argv 0]]
set seed [lindex $argv 1]
if {![string is integer -strict $seed] || $seed < 1} {
    error "seed must be a positive integer"
}

cd $out
project_open ap_core
set_global_assignment -name SEED $seed
export_assignments
project_close
