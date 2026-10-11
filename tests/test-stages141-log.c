#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../app/ui98/ui.h"
#include "of_save139.h"
#include "../app/ui98/gba_header140.h"
#include "../app/ui98/stages141_log.h"
static unsigned opens;static int failcode;static unsigned deny;
static int create_test(const char *path,uint32_t n){
 assert(strstr(path,"/Saves/chisburner/common/st141-")&&n==123);
 opens++;return opens<=deny?-4:failcode;
}
int main(void){
 char path[112];unsigned seq=0;
 deny=2;assert(cb_log141_open(create_test,path,sizeof path,1,&seq,123)==0&&opens==3&&seq==3);
 assert(strstr(path,"0001-0003.txt"));
 opens=0;deny=0;failcode=-12;
 assert(cb_log141_open(create_test,path,sizeof path,1,&seq,123)==-12&&opens==1);
 opens=0;deny=100;
 assert(cb_log141_open(create_test,path,sizeof path,1,&seq,123)==-4&&opens==16);
 opens=0;assert(cb_log141_open(create_test,path,2,1,&seq,123)==-11&&opens==0);
 struct cb_gba_header140 b={0},a={0};of_save141_diagnostic d={0};
 char out[8192];for(unsigned p=0;p<4;p++){b.blocks[p]=a.blocks[p]=6;memset(b.data[p],0xaa,192);memset(a.data[p],0xaa,192);}
 d.stage=7;d.probe_rc=d.restore_rc=-4;d.original=0x4c;d.target=0xb3;d.observed=0x5f;d.restored=0x9f;
 int n=cb_log141_format(out,sizeof out,3,1,-4,0,&b,&a,&d);
 assert(n>0&&n<8192&&strlen(out)==(size_t)n);
 assert(strstr(out,"restore_rc=-4")&&strstr(out,"original=4C target=B3 observed=5F restored=9F"));
 assert(strstr(out,"page=4 rc=0 blocks=6/6 diff_from_before0=0 comparable=1"));
 assert(strstr(out,"END diag.141-log\n"));
 a.rc[2]=-9;a.blocks[2]=2;a.data[2][0]=0;
 assert(cb_log141_format(out,sizeof out,0,0,0,0,&b,&a,&d)>0);
 assert(strstr(out,"executed=0")&&strstr(out,"page=3 rc=-9 blocks=2/6 diff_from_before0=1 comparable=0"));
 char tiny[17];memset(tiny,0x55,sizeof tiny);
 assert(cb_log141_format(tiny,16,0,0,0,0,&b,&a,&d)==-1&&tiny[16]==0x55);
 assert(cb_log141_format(tiny,0,0,0,0,0,&b,&a,&d)==-1);
 puts("PASS log formatting: all pages, restore mismatch, incomplete reads, skipped mode, bounds");
}
