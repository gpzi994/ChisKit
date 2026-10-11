#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "of_save139.h"
#define CB_NOR_IO -3
#define CB_NOR_UNSUPPORTED -2
#define CB_NOR_CANCELLED -4
static unsigned bank_calls,bank_fail,banks_seen[3];static bool removed;
static bool alive(void *ctx){(void)ctx;return !removed;}
static int sram138_bank(unsigned b){assert(bank_calls<3);banks_seen[bank_calls++]=b;return bank_calls==bank_fail?-8:0;}
static bool busy,private_owned;static struct {bool authorized;} job;
static unsigned job_token,mapper_kind,CART_CAPS=0x10008,calls,fail,entered,left;
static int enter(unsigned bus){assert(bus==2);entered++;private_owned=true;return 0;}
static int leave(void){left++;private_owned=false;return 0;}
static int save139_read(void *ctx,uint32_t a,uint8_t *v){
 (void)ctx;assert(busy&&private_owned&&!job.authorized&&a<65536);
 if(++calls==fail)return -9;
 *v=(uint8_t)a;return 0;
}
#include "../.integration/openfpgaos/src/firmware/os/kernel/save_read143.h"
int main(void){
 of_save143_readonly r;assert(!save_read143_kernel(&r)&&r.completed==64&&!r.differences&&calls==2112&&entered==1&&left==1&&!busy);
 for(fail=1;fail<=2112;fail++){
  calls=entered=left=0;assert(save_read143_kernel(&r)==-9&&calls==fail&&entered==1&&left==1&&!busy&&r.completed==(fail-1)/33);
 }
 busy=true;entered=0;assert(save_read143_kernel(&r)==-3&&!entered);busy=false;
 of_save144_readonly s;calls=entered=left=fail=0;
 assert(!save_read144_kernel(&s)&&s.scanned==65536&&s.sample.completed==64&&calls==67648&&!s.head_diff&&entered==1&&left==1);
 const unsigned failures[]={1,32,32768,65536,65537,67648};
 for(unsigned i=0;i<sizeof failures/sizeof failures[0];i++){
  fail=failures[i];calls=entered=left=0;
  assert(save_read144_kernel(&s)==-9&&calls==fail&&left==1&&!busy);
 }
 of_save145_banks b;calls=fail=bank_calls=0;
 assert(!save_read145_kernel(&b)&&calls==133184&&b.scanned[0]==65536&&b.scanned[1]==65536&&b.sample.completed==64&&!b.head_diff);
 assert(bank_calls==3&&banks_seen[0]==0&&banks_seen[1]==1&&banks_seen[2]==0);
 for(bank_fail=1;bank_fail<=3;bank_fail++){
  calls=bank_calls=0;assert(save_read145_kernel(&b)==-8&&!busy&&!private_owned);
 }
 bank_fail=0;bank_calls=calls=0;fail=32;
 assert(save_read145_kernel(&b)==-9&&bank_calls==2&&banks_seen[1]==0&&!b.restore_rc);
 bank_calls=calls=0;removed=true;
 assert(save_read145_kernel(&b)==-9&&bank_calls==1&&b.restore_rc==-4);
 puts("PASS read143/144/145: bank sequence, read/bank failures and removed-card recovery guard");
}
