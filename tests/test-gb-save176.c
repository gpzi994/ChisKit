/* Reuse the established GB-only bus/fault mock, never GBA save code. */
#define main legacy175_main
#include "test-gb-save175.c"
#undef main
int main(void){
 assert(!legacy175_main());
 struct gb175_plan p;
 setup(0x1b,3);assert(!gb176_plan(1,m.h,0,&p)&&p.bytes==131072&&p.banks==16);
 assert(!gb176_plan(1,m.h,131072,&p));
 assert(gb176_plan(2,m.h,0,&p)==GB175_UNSUPPORTED);
 setup(0x1e,3);assert(!gb176_plan(1,m.h,0,&p)&&p.bytes==65536);
 assert(gb176_plan(1,m.h,131072,&p)==GB175_UNSUPPORTED);
 setup(0x13,3);assert(!gb176_plan(1,m.h,0,&p)&&p.bytes==32768);
 assert(gb176_plan(1,m.h,65536,&p)==GB175_UNSUPPORTED);
 setup(0x10,5);assert(!gb176_plan(1,m.h,0,&p)&&p.banks==8);
 setup(0x20,3);assert(gb176_plan(1,m.h,0,&p)==GB175_UNSUPPORTED); /* MBC6 */
 setup(0x1b,3);m.bytes=131072;
 assert(!gb176_capture(&bus,1,m.h,0,snapshot,sizeof snapshot,scratch,512,&report));
 assert(report.compared==131072&&!memcmp(snapshot,m.ram,131072));
 assert(!m.bank&&!m.enabled&&!m.owned);
 assert(gb176_content_bytes(snapshot,131072)==131072);
 for(unsigned i=32768;i<131072;i++)snapshot[i]=snapshot[i%32768];
 assert(gb176_content_bytes(snapshot,131072)==32768);
 snapshot[0]^=1;assert(gb176_content_bytes(snapshot,131072)==131072);
 memset(snapshot,255,sizeof snapshot);assert(!gb176_content_bytes(snapshot,131072));
 memset(snapshot,0,sizeof snapshot);assert(!gb176_content_bytes(snapshot,131072));
 for(unsigned i=0;i<3;i++){
  setup(0x1b,3);m.bytes=131072;
  if(i==0)m.change_read=257;
  if(i==1)m.fail_read=256;
  if(i==2)m.remove_read=256;
  assert(gb176_capture(&bus,1,m.h,0,snapshot,sizeof snapshot,scratch,512,&report)<0);
  assert(!m.owned&&!m.creates);
 }
 puts("PASS GB176: MBC5 header32/read128, repetition, mapper guards and upper-bank failures");
 return 0;
}
