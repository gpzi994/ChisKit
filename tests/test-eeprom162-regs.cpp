#include "Vchisburner_eeprom_regs139.h"
#include <cstdint>
#include <cstdio>
#include <stdexcept>
static Vchisburner_eeprom_regs139 d;
static void check(bool v,const char *why){if(!v)throw std::runtime_error(why);}
static void tick(){d.clk=0;d.eval();d.clk=1;d.eval();}
static void write(unsigned addr,uint32_t value){d.wr=1;d.address=addr;d.data=value;tick();d.wr=0;tick();}
static void reset(){d.reset_n=0;tick();d.reset_n=1;d.lifecycle_ok=1;d.selected=1;
 d.other_busy=0;d.other_request=0;d.engine_busy=0;d.engine_done=0;
 d.engine_error=0;d.wr=0;d.read_address=3;tick();}
int main(){try{
 reset();write(1,63);write(4,0x01234567u);write(5,0x89abcdefu);
 d.wr=1;d.address=2;d.data=0x57524954u;tick();
 check(d.engine_start&&d.hold_owner&&d.write_mode&&!d.mode_large,"write launch");
 check(d.block_address==63&&d.write_data==0x0123456789abcdefULL,"write payload/address");
 d.wr=0;d.engine_busy=1;tick();d.engine_busy=0;d.engine_done=1;tick();
 check(!d.hold_owner&&(d.read_data&7u)==2u,"write completion");
 reset();write(1,0x4000u|1023u);write(4,0xdeadbeefu);write(5,0x01020304u);
 d.wr=1;d.address=2;d.data=0x57524954u;tick();
 check(d.engine_start&&d.mode_large&&d.block_address==1023,"large write launch");
 reset();write(1,64);write(4,0x12345678u);write(5,0x98765432u);
 d.wr=1;d.address=2;d.data=0x57524954u;tick();
 check(!d.engine_start&&!d.hold_owner&&(d.read_data&8u),"invalid small address");
 reset();write(1,0x4000u|1024u);
 d.wr=1;d.address=2;d.data=0x57524954u;tick();
 check(!d.engine_start&&!d.hold_owner&&(d.read_data&8u),"invalid large address");
 reset();write(1,1);d.other_busy=1;
 d.wr=1;d.address=2;d.data=0x57524954u;tick();
 check(!d.engine_start&&!d.hold_owner&&(d.read_data&8u),"other engine busy");
 reset();write(1,1);d.other_request=1;
 d.wr=1;d.address=2;d.data=0x57524954u;tick();
 check(!d.engine_start&&!d.hold_owner&&(d.read_data&8u),"simultaneous other request");
 reset();write(1,1);write(4,0xffffffffu);write(5,0);
 d.wr=1;d.address=2;d.data=1;tick();
 check(d.engine_start&&!d.write_mode,"legacy read changed to write");
 std::puts("EEPROM162 register PASS: payload, both modes, legacy read, range/owner guards");
 return 0;
}catch(const std::exception &e){std::fprintf(stderr,"FAIL: %s\n",e.what());return 1;}}
