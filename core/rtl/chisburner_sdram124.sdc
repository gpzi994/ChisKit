# Frozen first-fault metadata has at least three CPU clocks to settle.
# Observe only; this path must not become a control dependency.
set_net_delay -max 8.0 -from [sd121_regs {*|sd120|diagnostic|first_b[*]}] -to [sd121_regs {*|sd120|diagnostic|first_c[*]}]
