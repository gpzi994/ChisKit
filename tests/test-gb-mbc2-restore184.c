#define GB180_TEST_MAIN ordinary_tests180
#include "test-gb-restore180.c"
static int control184(void *v,uint16_t a,uint8_t d){assert(a==0);return control(v,a,d);}
static int ram184(void *v,uint16_t a,void *p,uint32_t n){
 assert(a>=0xa000&&a<0xa200&&n<=0xa200u-a);
 int rc=ram(v,a,p,n);
 /* Upper nibble varies electrically, including between comparison passes. */
 for(unsigned i=0;i<n;i++)((uint8_t*)p)[i]=(((uint8_t*)p)[i]&15)|((m.reads&15)<<4);
 return rc;
}
static int write184(void *v,uint16_t a,const void *p,uint32_t n){
 assert(a>=0xa000&&a<0xa200&&n<=0xa200u-a);
 for(unsigned i=0;i<n;i++)assert(((const uint8_t*)p)[i]<16);
 return writer(v,a,p,n);
}
static void init184(unsigned type){
 start180(type,0,512);
 for(unsigned i=0;i<512;i++){m.ram[i]&=15;snapshot[i]=m.ram[i];desired[i]=(m.ram[i]^5)|0xf0;}
}
static int run184(unsigned chunk){
 struct gb180_bus b={{&m,acquire,alive,cancelled,header,control184,ram184,release},write184};
 return gb180_restore(&b,m.h,snapshot,desired,512,scratch,chunk,&report);
}
int main(int argc,char **argv){
 for(unsigned t=5;t<=6;t++)for(unsigned chunk=1;chunk<=513;chunk+=16){
  init184(t);assert(!run184(chunk));assert(report.compared==512&&report.verified==512&&report.written==512);
  for(unsigned i=0;i<512;i++)assert(m.ram[i]==(desired[i]&15));
  assert(!m.owned&&!m.enabled&&!m.bank&&m.controls==4);
  init184(t);for(unsigned i=0;i<512;i++)desired[i]=snapshot[i]|0xf0;
  assert(!run184(chunk)&&!datawrites&&!report.written&&report.verified==512);
 }
 for(unsigned i=1;i<=1024;i++){
  init184(6);m.fail_read=i;assert(run184(1)<0&&!m.owned&&!m.enabled);
  if(i<=512)assert(!datawrites);
 }
 for(unsigned i=1;i<=512;i++){
  init184(6);failwrite=i;assert(run184(1)<0&&datawrites==i&&!m.owned&&!m.enabled);
  init184(6);remove_write=i;assert(run184(1)==GB175_CANCELLED&&!m.owned);
 }
 for(unsigned i=1;i<=4;i++){
  init184(6);m.fail_control=i;assert(run184(64)<0&&!m.owned);
  init184(6);m.change_header=i;assert(run184(64)==GB175_IDENTITY&&!m.owned);
  if(i<=2)assert(!datawrites);
 }
 init184(6);m.ram[511]^=1;assert(run184(64)==GB175_CHANGED&&!datawrites);
 init184(6);corruptwrite=8;assert(run184(64)==GB175_CHANGED&&report.verified==448);
 init184(6);m.release_error=1;assert(run184(512)==GB175_CLEANUP);
 init184(6);m.cancel_read=8;assert(run184(64)==GB175_CANCELLED&&!datawrites&&!m.enabled);
 for(unsigned fill=0;fill<2;fill++){
  init184(6);memset(m.ram,fill?15:0,512);memcpy(snapshot,m.ram,512);assert(!run184(512));
 }
 if(argc==2){
  init184(6);FILE *f=fopen(argv[1],"rb");assert(f);
  assert(fread(desired,1,512,f)==512&&fgetc(f)==EOF);fclose(f);
  memset(m.ram,15,512);memcpy(snapshot,m.ram,512);
  assert(!run184(512)&&report.verified==512);
  for(unsigned i=0;i<512;i++)assert(m.ram[i]==(desired[i]&15));
  puts("PASS actual Tenchi o Kurau 512-byte SAV fixture restored in MBC2 model");
 }
 puts("PASS GB184 MBC2: 512 low-nibble cells, no bank writes, full verify, upper-bit tolerance, no-op, blank, all byte faults, removal, identity and cleanup");
 return 0;
}
