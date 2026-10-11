#ifndef CB_RTC_RESTORE172_H
#define CB_RTC_RESTORE172_H
#include <stdint.h>
#include <stddef.h>
#include <string.h>
/* Pure file policy: never performs GPIO or save-memory writes.
 * Timestamps are UTC; RTC calendar fields retain the cartridge's wall time.
 * Advance means elapsed seconds, not host-local calendar relativedelta. */
struct cb_rtc172_value { uint8_t date[7],status; uint64_t timestamp; };
static inline int cb_rtc172_bcd(unsigned x){
 return (x&15)>9||(x>>4)>9?-1:(int)((x>>4)*10+(x&15));
}
static inline unsigned cb_rtc172_days(unsigned y,unsigned m){
 static const unsigned d[]={31,28,31,30,31,30,31,31,30,31,30,31};
 return d[m-1]+(m==2&&y%4==0);
}
/* 0 pure SAV, 1 valid tail, negative malformed/unsupported input. */
static inline int cb_rtc172_parse(const uint8_t *tail,size_t length,
 uint32_t save_bytes,struct cb_rtc172_value *out){
 if(!out||!(save_bytes==65536||save_bytes==131072))return -1;
 memset(out,0,sizeof *out);
 if(length==save_bytes)return 0;
 if(length!=(size_t)save_bytes+16||!tail)return -1;
 struct cb_rtc172_value v={0};memcpy(v.date,tail,7);
 v.status=tail[7]==1?0x40:tail[7]; /* FlashGBX legacy status */
 if(!(v.status&0x40)||(v.status&0x95))return -2;
 int y=cb_rtc172_bcd(v.date[0]),m=cb_rtc172_bcd(v.date[1]);
 int d=cb_rtc172_bcd(v.date[2]),h=cb_rtc172_bcd(v.date[4]&0x7f);
 int mi=cb_rtc172_bcd(v.date[5]),s=cb_rtc172_bcd(v.date[6]);
 if(y<0||m<1||m>12||d<1||v.date[3]>6||h<0||h>23||mi<0||mi>59||s<0||s>59)return -2;
 if((unsigned)d>cb_rtc172_days((unsigned)y,(unsigned)m))return -2;
 for(unsigned i=0;i<8;i++)v.timestamp|=(uint64_t)tail[8+i]<<(8*i);
 *out=v;return 1;
}
static inline uint8_t cb_rtc172_encode(unsigned n){return (uint8_t)((n/10)*16+n%10);}
/* No adjustment on a backwards host clock. Reject dates beyond 2099.
 * Failed adjustment leaves the caller's original value intact. */
static inline int cb_rtc172_advance(struct cb_rtc172_value *v,uint64_t now){
 if(!v||v->timestamp<946684800ULL||v->timestamp>4102444799ULL||
    now<946684800ULL||now>4102444799ULL)return -1;
 if(now<=v->timestamp)return 0;
 unsigned y=cb_rtc172_bcd(v->date[0]),m=cb_rtc172_bcd(v->date[1]),d=cb_rtc172_bcd(v->date[2]);
 if(y>99||m<1||m>12||d<1||d>cb_rtc172_days(y,m))return -1;
 int h=cb_rtc172_bcd(v->date[4]&0x7f),mi=cb_rtc172_bcd(v->date[5]),s=cb_rtc172_bcd(v->date[6]);
 if(h<0||h>23||mi<0||mi>59||s<0||s>59||v->date[3]>6)return -1;
 uint64_t total=(unsigned)h*3600u+(unsigned)mi*60u+(unsigned)s+now-v->timestamp;
 unsigned days=(unsigned)(total/86400),weekday=(v->date[3]+days)%7;
 unsigned seconds=(unsigned)(total%86400);
 while(days--){if(++d>cb_rtc172_days(y,m)){d=1;if(++m>12){m=1;if(++y>99)return -1;}}}
 v->date[0]=cb_rtc172_encode(y);v->date[1]=cb_rtc172_encode(m);v->date[2]=cb_rtc172_encode(d);
 v->date[3]=(uint8_t)weekday;h=seconds/3600;
 v->date[4]=cb_rtc172_encode((unsigned)h)|(h>=12?0x80:0);
 v->date[5]=cb_rtc172_encode(seconds/60%60);v->date[6]=cb_rtc172_encode(seconds%60);
 return 0;
}
static inline int cb_rtc172_prompt(int current_rtc_supported,int tail_result){
 return current_rtc_supported==1&&tail_result==1;
}
#endif
