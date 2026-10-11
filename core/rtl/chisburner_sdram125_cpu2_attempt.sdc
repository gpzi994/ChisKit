# Rejected experiment retained for reproducibility only. Adding this after
# chisburner_sdram125_cpu.sdc moved congestion into Agu/SRC2 and the cartridge
# engine; the resulting fit was not accepted or packaged.
set sd125_f2i [get_registers {*|cpu|cpu|execute_ctrl4_up_FpuF2iPlugin_logic_onShift_SHIFTED_lane0[*]}]
set sd125_src1 [get_registers {*|cpu|cpu|execute_ctrl2_up_early0_SrcPlugin_SRC1_lane0[*]}]
if {[get_collection_size $sd125_f2i] == 0 || [get_collection_size $sd125_src1] == 0} {
    error "SDRAM125 FpuF2i optimization endpoint selection is empty"
}
set_max_delay 10.500 -from $sd125_f2i -to $sd125_src1
