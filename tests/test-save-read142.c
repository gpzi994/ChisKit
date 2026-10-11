#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../app/ui98/save_read142.h"
static unsigned calls,fail,change;
static int read_test(unsigned a,void *dst,unsigned n){
 calls++;assert((a==0&&n==32)||(a==4&&n==1));
 if(calls==fail)return -9;
 for(unsigned i=0;i<n;i++)((unsigned char*)dst)[i]=(unsigned char)(a+i);
 if(calls==change)((unsigned char*)dst)[0]^=1;
 return 0;
}
int main(void){
 struct cb_read142 r;assert(cb_read142_run(&r,read_test)==0&&calls==128&&r.completed==64&&!r.differences);
 calls=0;change=4;assert(!cb_read142_run(&r,read_test)&&r.differences==1&&r.first_round==1&&r.first_address==4);
 change=0;
 for(fail=1;fail<=128;fail++){calls=0;assert(cb_read142_run(&r,read_test)==-9&&calls==fail&&r.completed==(fail-1)/2);}
 puts("PASS read-only 142: ranges, 64 pairs, mismatch, all 128 failure positions");
}
