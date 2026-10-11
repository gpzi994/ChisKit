#include "../core/gb_save175/gb_save175.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static const unsigned char logo[48]={0xce,0xed,0x66,0x66,0xcc,0x0d,0,0x0b,3,0x73,0,0x83,0,0x0c,0,0x0d,
0,8,0x11,0x1f,0x88,0x89,0,0x0e,0xdc,0xcc,0x6e,0xe6,0xdd,0xdd,0xd9,0x99,
0xbb,0xbb,0x67,0x63,0x6e,0x0e,0xec,0xcc,0xdd,0xdc,0x99,0x9f,0xbb,0xb9,0x33,0x3e};
static unsigned char snapshot[131072],scratch[8192];
struct mock {
 unsigned char h[80],ram[131072],file[131072];
 unsigned bytes,bank,enabled,owned,acquires,releases,headers,reads,controls,creates,writes,finished,aborts;
 unsigned fail_control,fail_read,change_header,change_read,remove_read,cancel_read;
 int acquire_error,release_error,header_error,create_error,write_error,finish_error,read_error,corrupt_file,removed,cancel;
};
static struct mock m;
static void setup(unsigned type,unsigned size){
 memset(&m,0,sizeof m);memcpy(m.h+4,logo,48);m.h[0x47]=(unsigned char)type;m.h[0x49]=(unsigned char)size;
 for(unsigned i=0x34;i<=0x4c;i++)m.h[0x4d]=(unsigned char)(m.h[0x4d]-m.h[i]-1);
 struct gb175_plan p;if(!gb175_plan_header(1,m.h,&p))m.bytes=p.bytes;
 for(unsigned i=0;i<sizeof m.ram;i++)m.ram[i]=(unsigned char)(i*17+i/8192);
}
static int acquire(void *v){struct mock *s=v;s->acquires++;if(s->acquire_error)return -77;s->owned=1;return 0;}
static int alive(void *v){return !((struct mock *)v)->removed;}
static int cancelled(void *v){return ((struct mock *)v)->cancel;}
static int header(void *v,uint8_t *h){struct mock *s=v;assert(s->owned&&!s->removed);s->headers++;if(s->header_error)return -77;
 memcpy(h,s->h,80);if(s->headers==s->change_header)h[0x34]^=1;return 0;}
static int control(void *v,uint16_t a,uint8_t d){struct mock *s=v;assert(s->owned&&!s->removed);s->controls++;
 assert(a==0||a==0x4000);if(a==0){assert(d==0||d==0x0a);s->enabled=d!=0;}
 else {assert(d<s->bytes/8192);s->bank=d;}
 return s->controls==s->fail_control?-77:0;
}
static int ram(void *v,uint16_t a,void *buf,uint32_t n){struct mock *s=v;
 assert(s->owned&&!s->removed&&s->enabled);assert(a>=0xa000&&a<=0xbfff&&n&&n<=0xc000u-a);
 unsigned off=s->bank*8192+a-0xa000;assert(off+n<=s->bytes);s->reads++;
 memcpy(buf,s->ram+off,n);if(s->reads==s->change_read)((unsigned char *)buf)[0]^=1;
 if(s->reads==s->remove_read)s->removed=1;
 if(s->reads==s->cancel_read)s->cancel=1;
 return s->reads==s->fail_read?-77:0;
}
static int release(void *v){struct mock *s=v;assert(s->owned);s->owned=0;s->releases++;return s->release_error?-77:0;}
static int create(void *v,uint32_t n){struct mock *s=v;assert(!s->owned&&s->releases==1&&n==s->bytes);s->creates++;return s->create_error?-77:0;}
static int write_file(void *v,uint32_t off,const void *buf,uint32_t n){struct mock *s=v;assert(off+n<=s->bytes);s->writes++;memcpy(s->file+off,buf,n);return s->write_error?-77:0;}
static int finish(void *v){struct mock *s=v;s->finished++;return s->finish_error?-77:0;}
static int read_file(void *v,uint32_t off,void *buf,uint32_t n){struct mock *s=v;assert(s->finished&&off+n<=s->bytes);memcpy(buf,s->file+off,n);if(s->corrupt_file)((unsigned char *)buf)[0]^=1;return s->read_error?-77:0;}
static void abort_file(void *v){((struct mock *)v)->aborts++;}
static struct gb175_bus bus={&m,acquire,alive,cancelled,header,control,ram,release};
static struct gb175_output out={&m,create,write_file,finish,read_file,abort_file};
static struct gb175_report report;
static int backup(unsigned b,unsigned chunk){return gb175_backup(&bus,&out,b,m.h,snapshot,sizeof snapshot,scratch,chunk,&report);}
static void failure(int expected){assert(backup(1,512)==expected);assert(!m.creates&&!m.owned);assert(m.releases==1);}
int main(void){
 struct gb175_plan p;unsigned bank;uint16_t addr;uint32_t len;
 /* Exhaust all type/size combinations; only the explicitly qualified set passes. */
 for(unsigned t=0;t<256;t++)for(unsigned r=0;r<256;r++){
  setup(t,r);int supported=((t==0x10||t==0x12||t==0x13)&&(r==2||r==3))||(t==0x10&&r==5)||
   ((t==0x1a||t==0x1b)&&(r>=2&&r<=5))||((t==0x1d||t==0x1e)&&(r==2||r==3||r==5));
  assert((gb175_plan_header(1,m.h,&p)==0)==supported);
 }
 setup(0x10,5);assert(!gb175_plan_header(1,m.h,&p)&&p.mapper==30&&p.banks==8);
 assert(!gb175_chunk(&p,8191,512,&bank,&addr,&len)&&bank==0&&addr==0xbfff&&len==1);
 assert(!gb175_chunk(&p,8192,512,&bank,&addr,&len)&&bank==1&&addr==0xa000&&len==512);
 assert(!gb175_chunk(&p,65535,~0u,&bank,&addr,&len)&&bank==7&&len==1);
 assert(gb175_chunk(&p,65536,512,&bank,&addr,&len)==GB175_ARGUMENT);
 assert(gb175_chunk(&p,~0u,512,&bank,&addr,&len)==GB175_ARGUMENT);
 assert(gb175_chunk(&p,0,0,&bank,&addr,&len)==GB175_ARGUMENT);
 assert(backup(2,512)==GB175_UNSUPPORTED&&!m.acquires);
 m.h[4]^=1;assert(backup(1,512)==GB175_IDENTITY&&!m.acquires);
 setup(0x1b,4);m.h[0x4d]^=1;assert(backup(1,512)==GB175_IDENTITY&&!m.acquires);
 const unsigned types[]={0x10,0x13,0x1b,0x1e},sizes[]={2,3,4,5};
 for(unsigned t=0;t<4;t++)for(unsigned r=0;r<4;r++)for(unsigned fill=0;fill<3;fill++){
  setup(types[t],sizes[r]);if(!m.bytes)continue;
  if(fill)memset(m.ram,fill==1?0:255,m.bytes);
  assert(!backup(1,777));assert(report.compared==m.bytes&&report.written==m.bytes&&report.verified==m.bytes);
  assert(!memcmp(m.file,m.ram,m.bytes)&&!m.enabled&&!m.bank&&m.releases==1&&m.headers==3);
 }
 setup(0x13,3);assert(gb175_backup(&bus,&out,1,m.h,snapshot,sizeof snapshot,snapshot+1,512,&report)==GB175_ARGUMENT&&!m.acquires);
 assert(gb175_backup(&bus,&out,1,m.h,snapshot,1,scratch,512,&report)==GB175_ARGUMENT&&!m.acquires);
 assert(backup(1,0)==GB175_ARGUMENT&&!m.acquires);assert(backup(1,8193)==GB175_ARGUMENT&&!m.acquires);
 setup(0x13,3);m.acquire_error=1;assert(backup(1,512)==GB175_IO&&!m.releases&&!m.creates);
 for(unsigned i=1;i<=3;i++){setup(0x13,3);m.change_header=i;failure(GB175_IDENTITY);}
 setup(0x13,3);m.header_error=1;failure(GB175_IO);
 /* Sweep every capture read and every control write, including cleanup. */
 for(unsigned i=1;i<=128;i++){setup(0x13,3);m.fail_read=i;failure(GB175_IO);assert(!m.enabled&&!m.bank);}
 for(unsigned i=1;i<=12;i++){setup(0x13,3);m.fail_control=i;failure(i<=10?GB175_IO:GB175_CLEANUP);}
 setup(0x13,3);m.change_read=65;failure(GB175_CHANGED);
 setup(0x13,3);m.release_error=1;failure(GB175_CLEANUP);
 for(unsigned i=1;i<=128;i++){
  setup(0x13,3);m.remove_read=i;failure(GB175_CANCELLED);
  setup(0x13,3);m.cancel_read=i;failure(GB175_CANCELLED);assert(!m.enabled&&!m.bank);
 }
 for(unsigned i=0;i<5;i++){
  setup(0x13,3);
  if(i==0)m.create_error=1;
  if(i==1)m.write_error=1;
  if(i==2)m.finish_error=1;
  if(i==3)m.read_error=1;
  if(i==4)m.corrupt_file=1;
  assert(backup(1,512)==GB175_FILE&&m.creates==1&&m.aborts==(i!=0));
 }
 puts("PASS gb_save175: exhaustive planner, bank boundaries, isolated read-only capture, fault/cleanup/file cases");
 return 0;
}
