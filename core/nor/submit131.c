// SPDX-License-Identifier: AGPL-3.0-only
#include "submit131.h"
enum { IDLE,LEASE,RUN,DRAIN,FINISHED };
static void put(struct cb_submit131 *s,unsigned r,uint32_t v){s->io.write(s->io.ctx,r,v);}
static uint32_t get(struct cb_submit131 *s,unsigned r){return s->io.read(s->io.ctx,r);}
static void stop(struct cb_submit131 *s,int result){
 s->result=result;
 put(s,CB_S131_REG_CONTROL,CB_S131_CANCEL);
 put(s,CB_S131_REG_LEASE,0);
 s->state=DRAIN;
}
int cb_submit131_begin(struct cb_submit131 *s,const struct cb_submit131_io *io,
 const struct cb_stream131_descriptor *d,uint32_t now,uint32_t timeout){
 if(!s||!io||!d||!io->read||!io->write||!io->check||!timeout||timeout>0x7fffffffu)return -1;
 if(s->state!=IDLE && s->state!=FINISHED)return -2;
 if(io->check(io->ctx))return -3;
 if((io->read(io->ctx,CB_S131_REG_LEASE)&7)||
    (io->read(io->ctx,CB_S131_REG_STATUS)&CB_S131_STATUS_BUSY))return -2;
 s->io=*io;s->descriptor=*d;s->started=now;s->timeout=timeout;s->result=0;s->state=LEASE;
 put(s,CB_S131_REG_LEASE,1);return 1;
}
int cb_submit131_step(struct cb_submit131 *s,uint32_t now,int cancel){
 if(!s || s->state==IDLE)return -1;
 if(s->state==FINISHED)return s->result;
 if(s->state!=DRAIN){
  if(cancel || s->io.check(s->io.ctx)){stop(s,-3);return 1;}
  if((uint32_t)(now-s->started)>=s->timeout){stop(s,-4);return 1;}
 }
 if(s->state==LEASE){
  if(!(get(s,CB_S131_REG_LEASE)&4))return 1;
  for(unsigned i=0;i<CB_S131_WORDS;i++){
   put(s,CB_S131_REG_INDEX,i);put(s,CB_S131_REG_DATA,s->descriptor.words[i]);
  }
  // Recheck immediately before arming, after descriptor register traffic.
  if(s->io.check(s->io.ctx)){stop(s,-3);return 1;}
  put(s,CB_S131_REG_KEY,CB_S131_KEY1);put(s,CB_S131_REG_KEY,CB_S131_KEY2);
  put(s,CB_S131_REG_CONTROL,CB_S131_SUBMIT);s->state=RUN;
 }else if(s->state==RUN){
  uint32_t status=get(s,CB_S131_REG_STATUS);
  if(status&CB_S131_STATUS_BUSY)return 1;
  if(status&CB_S131_STATUS_DONE)stop(s,(status&0xf8)?-5:0);
 }else if(s->state==DRAIN){
  if(!(get(s,CB_S131_REG_STATUS)&CB_S131_STATUS_BUSY) && !(get(s,CB_S131_REG_LEASE)&7)){
   s->state=FINISHED;return s->result;
  }
 }
 return 1;
}
