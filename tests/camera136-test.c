#include "../app/ui98/model.h"
#include "../app/ui98/camera136.h"
#include <assert.h>
#include <stdio.h>
struct fake {unsigned reads,safes,fail_read,fail_safe; int vary;};
static int read_fake(void *v,unsigned bus,uint32_t addr,void *out,unsigned n){
 struct fake *f=v;unsigned call=++f->reads;
 assert(bus==1&&n==32&&addr>=0x100&&addr<=0x140);
 assert(addr==0x100+((call-1)%4?((call-1)%4-1)*32:0));
 if(call==f->fail_read)return -9;
 for(unsigned i=0;i<n;i++)((uint8_t *)out)[i]=(uint8_t)(addr+i+(f->vary?(call-1)/4:0));
 return 0;
}
static int safe_fake(void *v){struct fake *f=v;return ++f->safes==f->fail_safe?-8:0;}
int main(void){
 struct fake f={0};struct cb_camera136_report r;
 struct cb_probe_ops o={&f,read_fake,safe_fake,0};
 cb_camera136_read(&r,&o);
 assert(f.reads==12&&f.safes==2&&r.completed==3);
 for(unsigned p=0;p<3;p++){assert(r.mask[p]==7&&r.rc[p]==0);for(unsigned i=0;i<96;i++)assert(r.bytes[p][i]==(uint8_t)i);}
 f=(struct fake){.vary=1};cb_camera136_read(&r,&o);
 assert(memcmp(r.bytes[0],r.bytes[1],96)&&memcmp(r.bytes[1],r.bytes[2],96));
 for(unsigned fail=1;fail<=12;fail++){
  f=(struct fake){.fail_read=fail};cb_camera136_read(&r,&o);
  assert(f.reads==fail&&f.safes==2&&r.rc[(fail-1)/4]==-9);
 }
 f=(struct fake){.fail_safe=1};cb_camera136_read(&r,&o);
 assert(!f.reads&&f.safes==1&&r.safe_before==-8&&!r.completed);
 f=(struct fake){.fail_safe=2};cb_camera136_read(&r,&o);
 assert(f.reads==12&&r.safe_after==-8);
 puts("PASS camera136: GB-only address bounds, 3 snapshots, unstable data, 12 read failures, safe failures");
}
