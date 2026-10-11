# Development fit only: prioritize the measured decode-to-fetch backpressure
# cone. This is a tighter optimization target, not a false/multicycle path.
# Native CPU period is 11.111 ns; require <=10.5 ns for this path group.
set sd125_decode [get_registers {*|cpu|cpu|decode_ctrls_2_up_RS*_PHYS_0[*]}]
set sd125_fetch [get_registers {*|cpu|cpu|fetch_logic_ctrls_*}]
if {[get_collection_size $sd125_decode] == 0 || [get_collection_size $sd125_fetch] == 0} {
    error "SDRAM125 CPU optimization endpoint selection is empty"
}
set_max_delay 10.500 -from $sd125_decode -to $sd125_fetch
