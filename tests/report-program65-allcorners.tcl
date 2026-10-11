# SPDX-License-Identifier: Apache-2.0
# Report the worst setup paths for every available PVT operating condition.
# Usage: quartus_sta -t report-program65-allcorners.tcl <project> <output-prefix>

set project_name [lindex $quartus(args) 0]
set output_prefix [lindex $quartus(args) 1]

project_open $project_name
create_timing_netlist -no_report
read_sdc

foreach_in_collection op [get_available_operating_conditions -all] {
    set_operating_conditions $op
    update_timing_netlist
    set model [get_operating_conditions_info $op -model]
    set voltage [get_operating_conditions_info $op -voltage]
    set temperature [get_operating_conditions_info $op -temperature]
    set output_file "${output_prefix}-${model}-${voltage}mV-${temperature}C.rpt"
    report_timing -setup -npaths 10 -detail summary -file $output_file
}

delete_timing_netlist
project_close
