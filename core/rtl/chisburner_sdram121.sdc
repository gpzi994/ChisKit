# Development-only routing bounds, independent of synchronous setup/hold.
# Both clocks have periods >=10 ns. Bounding each Gray bit below 8 ns also
# prevents adjacent source transitions overtaking one another in routing.
# report_net_delay MUST contain nonempty positive-slack paths after fit.
# Capture-end selection retains bit 10 when synthesis merges Gray MSB with
# the identical binary-counter MSB. Only actual direct nets to stage 1 match.
proc sd121_regs {pattern} {
    set nodes [get_registers $pattern]
    if {[get_collection_size $nodes] == 0} { error "SD121 missing CDC registers: $pattern" }
    return $nodes
}
foreach {launch capture} {
    {*|sd120|writer|*} {*|sd120|writer|wr_d1[*]}
    {*|sd120|writer|*} {*|sd120|writer|rd_s1[*]}
    {*|sd120|writer|*} {*|sd120|writer|done_s1[*]}
    {*|sd120|reader|transport|byte_count[*]} {*|sd120|reader|transport|bridge_count[*]}
    {*|sd120|reader|transport|publish_valid} {*|sd120|reader|transport|bridge_ready}
} {
    set_net_delay -max 8.0 -from [sd121_regs $launch] -to [sd121_regs $capture]
}
