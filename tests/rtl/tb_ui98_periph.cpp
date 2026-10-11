#define main unused_upstream_main
#include "../../.integration/openfpgaos/src/fpga/test/tb_axi_periph_main.cpp"
#undef main
static unsigned address(unsigned reg){return reg==16?0x4000015c:reg>=17?0x40000184+4*(reg-17):0x400001a0+4*reg;}
static void write(unsigned reg,unsigned value){axi_write_single(address(reg),value);}
static unsigned read(unsigned reg){return mmio_read32(address(reg));}
int main(int argc,char **argv){
 Verilated::commandArgs(argc,argv);tb=new Vtb_axi_periph;reset_sequence();
#ifdef TEST_EEPROM139
 const unsigned ee_addresses[]={0x40000074,0x40000078,0x4000007c,0x400000d8};
 for(unsigned i=0;i<4;i++){
  unsigned before=tb->dbg_cart_wr_count;
  axi_write_single(ee_addresses[i],0x13570000+i);
  check_eq("EEPROM AXI write count",tb->dbg_cart_wr_count,(before+1)&255);
  check_eq("EEPROM logical index",tb->dbg_cart_last_addr,27+i);
  check_eq("EEPROM write payload",tb->dbg_cart_last_wdata,0x13570000+i);
  before=tb->dbg_cart_rd_count;(void)mmio_read32(ee_addresses[i]);
  check_eq("EEPROM AXI read count",tb->dbg_cart_rd_count,(before+1)&255);
 }
 const unsigned isolated[]={0x40000070,0x40000080,0x400000d4,0x400000dc,
                           0x40000174,0x40000178,0x4000017c,0x400001e0,
                           0x400001e4,0x400001e8,0x400001ec,0x400001f0,
                           0x40000274,0x40000278,0x4000027c,0x400002d8,
                           0x40000378,0x40000474,0x40010074};
 for(unsigned a:isolated){
  unsigned before=tb->dbg_cart_rd_count;(void)mmio_read32(a);
  check_eq("EEPROM does not alias adjacent/input/storage/high page",tb->dbg_cart_rd_count,before);
 }
 // Write isolation at unmapped high aliases (avoid writes to live neighbors).
 for(unsigned a:{0x40000274u,0x40000278u,0x4000027cu,0x400002d8u,
                 0x40000474u,0x40010074u}){
  unsigned before=tb->dbg_cart_wr_count;axi_write_single(a,1);
  check_eq("EEPROM high write cannot submit",tb->dbg_cart_wr_count,before);
 }
#else
 for(unsigned a:{0x40000074u,0x40000078u,0x4000007cu,0x400000d8u}){
  unsigned before=tb->dbg_cart_wr_count;axi_write_single(a,0);
  check_eq("EEPROM disabled no routing",tb->dbg_cart_wr_count,before);
 }
#endif
#ifdef TEST_PROGRAM131
 test_input_hub();
 tb->cont1_key_in=0x13579bdf;tb->cont1_joy_in=0x2468ace0;tb->cont1_trig_in=0x1234;
 tb->cont2_key_in=0x31415926;tb->cont2_joy_in=0x27182818;tb->cont2_trig_in=0x5678;
 for(int i=0;i<8;i++)tick();
 const uint32_t expected_slots[]={0x13579bdf,0x2468ace0,0x1234,0x31415926,0x27182818,0x5678};
 std::vector<uint32_t> slot_reads;
 if(axi_read_burst(0x40000050,5,slot_reads,0xaaaa))
  for(unsigned i=0;i<6;i++)check_eq("legacy controller burst snapshot",slot_reads[i],expected_slots[i]);
 check_eq("slot1 snapshot key",mmio_read32(0x40000114),expected_slots[0]);
 check_eq("slot2 snapshot key",mmio_read32(0x40000124),expected_slots[3]);
 axi_write_single(0x400001f4,0x43425331);
 check_eq("stream key logical index",tb->dbg_cart_last_addr,24);
 check_eq("stream key data",tb->dbg_cart_last_wdata,0x43425331);
 axi_write_single(0x400001f8,1);
 check_eq("stream lease logical index",tb->dbg_cart_last_addr,25);
 unsigned before=tb->dbg_cart_rd_count;
 (void)mmio_read32(0x400001f8);
 check_eq("stream lease read routed",tb->dbg_cart_rd_count,(before+1)&255);
 before=tb->dbg_cart_wr_count;
 axi_write_single(0x400001f0,0);
 check_eq("storage trace not cartridge",tb->dbg_cart_wr_count,before);
#endif
#ifdef TEST_SDRAM118
 check_eq("SD118 signature",mmio_read32(0x40000200),0x53443116);
 axi_write_single(0x4e000000,1);
 axi_write_single(0x40000208,0x10004000);
 axi_write_single(0x4000020c,264);
 check_eq("SD118 source",mmio_read32(0x40000208),0x10004000);
 check_eq("SD118 size",mmio_read32(0x4000020c),264);
 check_eq("CRAM owner preserved",mmio_read32(0x4e000000),1);
 axi_write_single(0x40000308,0xdeadbeef);
 check_eq("No high-offset alias",mmio_read32(0x40000208),0x10004000);
 axi_write_single(0x40000210,8);
 check_eq("Reset command rejected",mmio_read32(0x40000204)&16,16);
 axi_write_single(0x40000210,4);
 check_eq("Rejected flag cleared",mmio_read32(0x40000204)&16,0);
 check_eq("CRAM owner still preserved",mmio_read32(0x4e000000),1);
 axi_write_single(0x4e000000,0);
#endif
 #ifdef TEST_GPU_LEAN132
 check_eq("renderer features absent",mmio_read32(0x40000098)&0x7efffc10u,0);
 axi_write_single(0x4a000000,2);
 check_eq("absent GPU boot status idle",mmio_read32(0x4a000014),0);
 check_eq("absent GPU fence is not manufactured",mmio_read32(0x4a000018),0);
 tb->vsync_in=0;tb->gpu_swap_req_in=0;for(int i=0;i<8;i++)tick();
 axi_write_single(0x40000018,3);
 check_eq("CPU flip pending without GPU",mmio_read32(0x40000018)&1u,1);
 tb->vsync_in=1;for(int i=0;i<16;i++)tick();
 check_eq("CPU flip completes on vsync",mmio_read32(0x40000018)&7u,2);
 tb->vsync_in=0;
 check_eq("save and cartridge features retained",mmio_read32(0x40000098)&0x01000220u,0x01000220u);
 #else
 check_eq("renderer remains in normal build",mmio_read32(0x40000098)&16u,16u);
 #endif
 check_eq("combined capabilities",read(1),0x02313f03);
 for(unsigned round=0;round<3;round++)for(unsigned bus=1;bus<=2;bus++){
  write(3,1);write(4,bus);write(16,0);check_eq("bus readback after staged write",read(4),bus);
  write(5,bus==1?0x100:0);write(6,32);write(7,100000);write(3,2);
  write(4,bus==1?2:1);check_eq("busy selector held",read(4),bus);
  unsigned status=0;for(int i=0;i<2000;i++){status=read(2);if(!(status&2))break;}
  check_eq("completed",status&0xf,5);check_eq("read length",read(9),32);
  for(int i=0;i<8;i++)check_eq("FIFO pop through AXI",read(8),bus==1?0x5a5a5a5a:0xa55aa55a);
  check_eq("FIFO empty",read(9),0);write(3,1);check_eq("safe status",read(2),1);
 }
#ifdef TEST_GB_IDENTIFY134
 write(4,1);write(16,4);write(5,0);write(6,32);write(7,100000);write(3,2);
 for(int i=0;i<100&&(read(2)&2);i++){}
 check_eq("retired GB identify rejected",read(2),9);
 check_eq("retired GB identify no payload",read(9),0);
 write(3,1);
#endif
 write(4,2);write(16,4);write(5,0);write(6,32);write(7,100000);write(3,2);
 for(int i=0;i<3000&&(read(2)&2);i++){}
 check_eq("unknown NOR cannot authorize",read(20),0);
#ifdef TEST_GBA_IDENTIFY133
 check_eq("fixed GBA identify rejected",read(2)&8,8);
 check_eq("fixed identify produces no payload",read(9),0);
 write(3,1);write(4,2);write(16,0);write(5,0);write(6,32);write(7,1);write(3,2);
 for(int i=0;i<100&&(read(2)&2);i++){}
 check_eq("GBA read timeout retained",read(2)&0x29,0x29);
 write(3,1);write(4,2);write(16,0);write(5,0);write(6,32);write(7,100000);write(3,2);write(3,3);
 for(int i=0;i<100&&(read(2)&2);i++){}
 check_eq("GBA abort retained",read(2)&0x13,0x11);
 write(3,1);write(4,2);write(16,0);write(5,0);write(6,32);write(7,100000);write(3,2);
 for(int i=0;i<2000&&(read(2)&2);i++){}
 check_eq("GBA reread after abort",read(2)&15,5);
 check_eq("GBA full payload after recovery",read(9),32);
#endif
 write(3,1);write(4,1);write(16,0);check_eq("return GB selected",read(4),1);
 printf("UI98 staged AXI + real cartridge router: %d passed, %d failed\n",passes,fails);
 delete tb;return fails?1:0;
}
