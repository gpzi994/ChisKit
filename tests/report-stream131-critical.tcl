package require ::quartus::project
package require ::quartus::sta

set root [file normalize [file join [file dirname [info script]] ..]]
set fit_dir [file join $root build stream131 fit-seed6]
cd $fit_dir
project_open ap_core
create_timing_netlist
read_sdc
update_timing_netlist

set ram_clk {ic|mp_ram|altera_pll_i|general[0].gpll~PLL_OUTPUT_COUNTER|divclk}
report_timing \
    -setup \
    -from_clock $ram_clk \
    -to_clock $ram_clk \
    -npaths 20 \
    -detail full_path \
    -file [file join $fit_dir stream131-critical-setup.txt]

delete_timing_netlist
project_close
