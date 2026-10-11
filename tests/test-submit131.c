#include "submit131.h"
#include <assert.h>
#include <stdio.h>
struct model{uint32_t lease,status;unsigned index,words,keys,submits,cancels;int revoked;};
static uint32_t rd(void *p,unsigned r){struct model*m=p;return r==25?m->lease:m->status;}
static void wr(void *p,unsigned r,uint32_t v){struct model*m=p;
 if(r==20)m->index=v;
 if(r==21){assert(m->index==m->words);m->words++;}
 if(r==24){assert(v==(m->keys?CB_S131_KEY2:CB_S131_KEY1));m->keys++;}
 if(r==22 && v==1){assert(m->keys==2 && m->words==16);m->submits++;m->status=1;}
 if(r==22 && v==2)m->cancels++;
 if(r==25)m->lease=v?3:2; // Release is asynchronous.
}
static int check(void*p){return ((struct model*)p)->revoked;}
int main(void){
 struct cb_stream131_descriptor d={0};
 for(unsigned scenario=0;scenario<5;scenario++){
  struct model m={0};struct cb_submit131 s={0};struct cb_submit131_io io={&m,rd,wr,check};
  assert(cb_submit131_begin(&s,&io,&d,0xfffffff0u,100)==1);
  assert(cb_submit131_begin(&s,&io,&d,0,100)==-2);
  if(scenario==1)m.revoked=1;
  if(scenario==2){assert(cb_submit131_step(&s,90,0)==1);}
  else {
   m.lease=7;assert(cb_submit131_step(&s,1,scenario==3)==1);
   if(!scenario || scenario==4){assert(m.submits==1);m.status=scenario==4?0x52:2;
    assert(cb_submit131_step(&s,2,0)==1);}
  }
  assert(m.cancels==1);
  assert(cb_submit131_step(&s,1000,0)==1); // Never return while leased.
  m.status=0;m.lease=0;
  int result=cb_submit131_step(&s,1001,0);
  assert(result==(scenario==0?0:scenario==2?-4:scenario==4?-5:-3));
  assert(m.submits==((scenario==0||scenario==4)?1u:0u));
 }
 puts("PASS submit131: completion, revocation, timeout wrap, cancel, hardware error and drain");
}
