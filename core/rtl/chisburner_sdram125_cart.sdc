# Tighten the one cart-read path that remained marginal after the bounded
# APF-tail fix. This is stricter than the 90 MHz 11.111 ns clock period and
# therefore guides placement/routing without waiving functional timing.
set sd125_cart_addr [get_registers {*|legacy|e1_addr_q[1]}]
set sd125_periph_rd30 [get_registers {*|periph_rd_data_r[30]}]
if {[get_collection_size $sd125_cart_addr] == 0} {
    error "SDRAM125 cart timing launch register was not found"
}
if {[get_collection_size $sd125_periph_rd30] == 0} {
    error "SDRAM125 peripheral timing capture register was not found"
}
set_max_delay 10.700 -from $sd125_cart_addr -to $sd125_periph_rd30
