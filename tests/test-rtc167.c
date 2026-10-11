#include <assert.h>
#include <stdio.h>
#include "../core/rtc/read167.h"
#include "../app/ui98/rtc167_ui.h"
struct mock {unsigned re,dir,data,cmd,nbits,bit,output,calls,fail,drop,commands,register_writes;int live,absent,write_only_control,allow_write,corrupt;uint8_t status,date[7];};
static int live(void *p){return ((struct mock*)p)->live;}
static int access_bus(struct mock *m){
 assert(m->live);m->calls++;
 if(m->calls==m->drop){m->live=0;return -4;}
 return m->calls==m->fail?-77:0;
}
static int rd(void *p,uint32_t a,uint16_t *v){
 struct mock *m=p;int rc=access_bus(m);if(rc)return rc;
 assert(a==0xc4||a==0xc6||a==0xc8);
 static const uint16_t rom[]={0x1234,0x5678,0x9abc};
 if(!m->re||m->absent)*v=rom[(a-0xc4)/2];
 else *v=a==0xc8?(m->write_only_control?0:1):a==0xc6?(m->write_only_control?0:m->dir):((m->data&5)|(m->output<<1));
 return 0;
}
static int wr(void *p,uint32_t a,uint16_t v){
 struct mock *m=p;int rc=access_bus(m);if(rc)return rc;
 assert(a==0xc4||a==0xc6||a==0xc8);
 if(m->absent)return 0;
 if(a==0xc8){assert(v<=1);m->re=v;return 0;}
 if(a==0xc6){assert(v==0||v==5||v==7);m->dir=v;return 0;}
 assert(v<=7);
 if(!(v&4)){m->nbits=m->cmd=m->bit=0;}
 else if(!(m->data&1)&&(v&1)){
  if(m->dir==7){
   if(m->nbits<8){m->cmd=(m->cmd<<1)|((v>>1)&1);m->nbits++;if(m->nbits==8){assert(m->cmd==0x63||m->cmd==0x65||(m->allow_write&&(m->cmd==0x62||m->cmd==0x64)));m->commands++;}}
   else {
    assert(m->allow_write&&(m->cmd==0x62||m->cmd==0x64));
    unsigned i=m->bit++;assert(i<(m->cmd==0x62?8u:56u));
    uint8_t *b=m->cmd==0x62?&m->status:&m->date[i/8];
    *b=(uint8_t)((*b&~(1u<<(i%8)))|(((v>>1)&1)<<(i%8)));m->register_writes++;
   }
  }
  else if(m->dir==5&&m->nbits==8){
   unsigned i=m->bit++;assert(i<(m->cmd==0x63?8u:56u));
   uint8_t b=m->cmd==0x63?m->status:m->date[i/8];
   if(m->corrupt&&m->register_writes&&m->cmd==0x65&&i/8==6)b=m->corrupt==2?0x11:0x12;
   m->output=(b>>(i%8))&1;
  }
 }
 m->data=v;return 0;
}
static struct mock init(void){struct mock m={0};m.live=1;m.status=0x40;uint8_t d[]={0x24,0x02,0x29,0x04,0x23,0x59,0x58};memcpy(m.date,d,7);return m;}
int main(void){
 struct mock m=init();struct cb_rtc167_ops o={&m,rd,wr,live};of_rtc167_report r;char s[28];
 assert(!cb_rtc167_capture(&o,&r));assert(r.flags==(OF_RTC167_GPIO|OF_RTC167_TIME_OK));
 assert(!memcmp(r.date,m.date,7)&&!m.re&&m.commands==2);unsigned calls=m.calls;
 cb_rtc167_format(s,sizeof s,0,&r);assert(!strcmp(s,"2024-02-29 23:59:58"));
 m.date[6]=0x59;assert(!cb_rtc167_capture(&o,&r));cb_rtc167_format(s,sizeof s,0,&r);assert(!strcmp(s,"2024-02-29 23:59:59"));
 m=init();m.write_only_control=1;assert(!cb_rtc167_capture(&o,&r));
 assert((r.flags&(OF_RTC167_GPIO|OF_RTC167_TIME_OK))==(OF_RTC167_GPIO|OF_RTC167_TIME_OK));
 assert(r.mapped[1]==0&&r.mapped[2]==0&&m.commands==2);
 m=init();m.absent=1;assert(!cb_rtc167_capture(&o,&r));assert(!r.flags&&!m.commands);cb_rtc167_format(s,sizeof s,0,&r);assert(!strcmp(s,"Not detected"));
 m=init();m.status=0xc0;assert(!cb_rtc167_capture(&o,&r));assert((r.flags&OF_RTC167_POWER_FLAG)&&!(r.flags&OF_RTC167_TIME_OK));
 m=init();m.date[0]=0x23;assert(!cb_rtc167_capture(&o,&r)&&!(r.flags&OF_RTC167_TIME_OK));
 m=init();m.date[5]=0x6a;assert(!cb_rtc167_capture(&o,&r)&&!(r.flags&OF_RTC167_TIME_OK));
 m=init();m.status=0;assert(!cb_rtc167_capture(&o,&r)&&!(r.flags&OF_RTC167_TIME_OK));
 for(unsigned i=1;i<=calls;i++){
  m=init();m.fail=i;assert(cb_rtc167_capture(&o,&r));assert(!(r.flags&OF_RTC167_TIME_OK));
  m=init();m.drop=i;assert(cb_rtc167_capture(&o,&r));assert(m.calls==i&&!m.live);
 }
 cb_rtc167_format(s,sizeof s,-77,&r);assert(!strcmp(s,"Read failed rc -77"));
 puts("RTC167 PASS: FlashGBX status-first probe, write-only control readback, MSB commands, LSB dates, GPIO absence, BCD/leap/power flags, all bus faults and lifecycle loss; mock hardware only");
 return 0;
}
