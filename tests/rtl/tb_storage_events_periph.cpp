// Exercise the actual AXI slave's diagnostic address decode and side effects.
#define main unused_upstream_main
#include "../../.integration/openfpgaos/src/fpga/test/tb_axi_periph_main.cpp"
#undef main
int main(int argc,char **argv) {
    Verilated::commandArgs(argc,argv); tb=new Vtb_axi_periph;
    tb->storage_trace_in=0; tb->events_status_in=1;
    tb->events_count_in=0x70100u; tb->events_data_in=0xdf3f1c60u;
    reset_sequence();
#ifdef AUDIT_TEST
    check_eq("audit identity",mmio_read32(0x400001e0u),0x46474237u);
    check_eq("identity arms once",tb->events_arms,1);
#else
    check_eq("event identity",mmio_read32(0x400001e0u),0x46474236u);
#endif
    check_eq("count flags",mmio_read32(0x400001e8u),0x70100u);
    check_eq("no freeze from metadata",tb->events_freezes,0);
    check_eq("no pop from metadata",tb->events_pops,0);
    check_eq("freeze status",mmio_read32(0x400001e4u),1);
    check_eq("one freeze strobe",tb->events_freezes,1);
    for(unsigned i=0;i<10;++i) {
        std::vector<uint32_t> out;
        bool ok=axi_read_burst(0x400001ecu,0,out,1);
        check_eq("stalled read completes",ok,1);
        check_eq("raw high-bit data",ok?out[0]:0,0xdf3f1c60u);
    }
    check_eq("one pop per accepted read",tb->events_pops,10);
    axi_write_single(0x400001ecu,0xffffffffu);
    axi_write_single(0x400001e4u,0xffffffffu);
    check_eq("writes never pop",tb->events_pops,10);
    check_eq("writes never freeze",tb->events_freezes,1);
    check_eq("public status unchanged",mmio_read32(0x4000003cu),0x60);
    check_eq("legacy trace preserved",mmio_read32(0x400001f0u),0xd0000060u);
#ifdef AUDIT_TEST
    check_eq("other reads never arm",tb->events_arms,1);
    axi_write_single(0x400001e0u,0xffffffffu);
    check_eq("write to ID never arms",tb->events_arms,1);
#endif
    printf("Event periph: %d passed, %d failed\n",passes,fails);
    delete tb; return fails?1:0;
}
