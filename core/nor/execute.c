// SPDX-License-Identifier: AGPL-3.0-only
// Portable adaptation of Mori GBA/GB flash command sequences and polling.
// Special M36L0R806 single-write flow follows Mori's FlashGBX profile path.
#include "execute.h"
#define TRY(x) do { rc=(x);if(rc)goto done; } while(0)
static uint16_t swap01(uint16_t v){return (uint16_t)((v&0xfffcu)|((v&1u)<<1)|((v&2u)>>1));}
static uint16_t cmd(const struct cb_nor_job*j,uint16_t v){return j->plan->recipe.swapped?swap01(v):v;}
static bool intel(const struct cb_nor_job*j){return j->plan->geometry.cmdset==CHISBURNER_NOR_CMDSET_INTEL;}
static bool dual(const struct cb_nor_job*j){return j->plan->recipe.method==CB_NOR_INTEL_DUAL;}
static int guard(struct cb_nor_job*j){
 if(!j||!j->plan||!j->io.select||!j->io.read||!j->io.write||!j->io.now_ms||
    !j->io.yield||!j->io.alive||!j->io.cancelled||!j->io.release)return CB_NOR_ARGUMENT;
 if(!j->authorized)return CB_NOR_UNARMED;
 if(!j->io.alive(j->io.ctx)||j->io.cancelled(j->io.ctx))return CB_NOR_CANCELLED;
 return 0;
}
static int reject(struct cb_nor_job*j,int rc){
 if(j){j->authorized=false;if(j->io.release)j->io.release(j->io.ctx);}
 return rc;
}
static int write_bus(struct cb_nor_job*j,uint32_t a,uint16_t v){
 if(!j->io.alive(j->io.ctx))return CB_NOR_CANCELLED;
 return j->io.write(j->io.ctx,a,v)?CB_NOR_IO:0;
}
static int read_bus(struct cb_nor_job*j,uint32_t a,uint16_t*v){
 if(!j->io.alive(j->io.ctx))return CB_NOR_CANCELLED;
 return j->io.read(j->io.ctx,a,v)?CB_NOR_IO:0;
}
static uint32_t physical(const struct cb_nor_job*j,uint32_t a){
 // Mori burner_mbc5_addr_to_program_window: bank 0 is the fixed window.
 return j->plan->recipe.unit==1?(a<0x4000?a:0x4000+(a&0x3fff)):a&0x1ffffff;
}
static int select_bus(struct cb_nor_job*j,uint32_t a){
 if(!j->io.alive(j->io.ctx))return CB_NOR_CANCELLED;
 if(j->io.select(j->io.ctx,a)){
  /* A failed latch transaction can leave a different/unknown window.
   * Do not send even cleanup reset commands through that route. */
  j->authorized=false;return CB_NOR_UNSUPPORTED;
 }
 return 0;
}
static int reset(struct cb_nor_job*j,uint32_t address){
 int rc=0;
 unsigned dies=dual(j)?2:1;
 for(unsigned i=0;i<dies;i++){
  if(intel(j)){
   rc=write_bus(j,address+i*2,cmd(j,0x50));if(rc)return rc;
  }
  rc=write_bus(j,address+i*2,j->plan->recipe.reset);if(rc)return rc;
 }
 return 0;
}
static int end(struct cb_nor_job*j,int rc){
 /* Soft cancellation still resets the current chip mode. Lifecycle loss
  * issues no more bus writes. All failures revoke the entire write job. */
 int cleanup=j->authorized?reset(j,0):0;
 j->io.release(j->io.ctx);
 if(!rc)rc=cleanup;
 if(rc)j->authorized=false;
 return rc;
}
static int unlock(struct cb_nor_job*j){
 int rc=write_bus(j,j->plan->recipe.unlock0,j->plan->recipe.aa);
 return rc?rc:write_bus(j,j->plan->recipe.unlock1,j->plan->recipe.bb);
}
static int poll(struct cb_nor_job*j,uint32_t a,uint16_t expected,uint32_t timeout){
 uint32_t started=j->io.now_ms(j->io.ctx);
 for(unsigned spins=0;spins<1000000;spins++){
  uint16_t s,t;
  int rc=guard(j);if(rc)return rc;
  if((uint32_t)(j->io.now_ms(j->io.ctx)-started)>=timeout)return CB_NOR_TIMEOUT;
  rc=read_bus(j,a,&s);if(rc)return rc;j->last_status=s;
  if(intel(j)){
   uint16_t normalized=j->plan->recipe.swapped?swap01(s):s;
   if(normalized&0x80)return normalized&0x3a?CB_NOR_STATUS:0;
  }else if(j->plan->recipe.method==CB_NOR_MSP55_AAA||
           j->plan->recipe.method==CB_NOR_MSP55_5555){
   /* MSP wide profiles wait for the complete PD/FFFF word (FlashGBX
    * MSP55LV100S buffer_write_wait_for / sector_erase_wait_for). A ready
    * low byte does not establish that the high byte has finished.
    * DQ5 is meaningful only on a lane whose DQ7 is still not ready;
    * completed-lane ROM data may legitimately contain bit 5. */
   if(s==expected){
    rc=read_bus(j,a,&t);if(rc)return rc;j->last_status=t;
    if(t==expected)return 0;
   }
   uint16_t pending=(uint16_t)((s^expected)&0x8080);
   uint16_t failed=(uint16_t)(((s&0x2020)<<2)&pending);
   if(failed){
    rc=read_bus(j,a,&t);if(rc)return rc;j->last_status=t;
    if((t^expected)&failed)return CB_NOR_STATUS;
   }
  }else{
   if((s&0x80)==(expected&0x80)){
    rc=read_bus(j,a,&t);if(rc)return rc;j->last_status=t;
    if((t&0x80)==(expected&0x80))return 0;
   }
   if(s&0x20){
    rc=read_bus(j,a,&t);if(rc)return rc;j->last_status=t;
    return (t&0x80)==(expected&0x80)?0:CB_NOR_STATUS;
   }
  }
  j->io.yield(j->io.ctx);
 }
 return CB_NOR_TIMEOUT; /* Broken clock cannot hang kernel forever. */
}
static int verify(struct cb_nor_job*j,uint32_t a,const uint8_t*data,size_t len){
 unsigned unit=j->plan->recipe.unit;
 for(size_t n=0;n<len;n+=unit){
  uint16_t value,expected=data?(uint16_t)(data[n]|(unit==2?(uint16_t)data[n+1]<<8:0)):(unit==2?0xffff:0xff);
  int rc=guard(j);if(rc)return rc;
  rc=select_bus(j,a+(uint32_t)n);if(rc)return rc;
  rc=read_bus(j,physical(j,a+(uint32_t)n),&value);if(rc)return rc;
  if((unit==2?value:(value&0xff))!=expected){
   j->failed_address=a+(uint32_t)n;return CB_NOR_VERIFY;
  }
  if((n&1023)==0)j->io.yield(j->io.ctx);
 }
 return 0;
}
static int erase_impl(struct cb_nor_job*j,uint32_t a,uint32_t timeout,bool readback){
 uint32_t start,len,pa;
 int rc=guard(j);
 if(rc)return reject(j,rc);
 if(!timeout||timeout>120000||cb_nor_sector_at(j->plan,a,&start,&len)||start!=a)return reject(j,CB_NOR_ARGUMENT);
 if(a>=j->plan->accessible_bytes||len>j->plan->accessible_bytes-a)return reject(j,CB_NOR_ARGUMENT);
 if(dual(j)&&(a&3))return reject(j,CB_NOR_ARGUMENT);
 pa=physical(j,a);j->failed_address=a;
 rc=select_bus(j,a);if(rc)return reject(j,rc);
 TRY(reset(j,0));
 if(intel(j)){
  for(unsigned die=0;die<(dual(j)?2u:1u);die++){
   uint32_t sa=pa+die*2;
   if(!j->plan->skip_block_unlock){
    TRY(write_bus(j,sa,cmd(j,0x60)));TRY(write_bus(j,sa,cmd(j,0xd0)));
   }
   TRY(write_bus(j,sa,j->plan->recipe.erase));TRY(write_bus(j,sa,j->plan->recipe.sector));
  }
  for(unsigned die=0;die<(dual(j)?2u:1u);die++)TRY(poll(j,pa+die*2,0,timeout));
 }else{
  TRY(unlock(j));TRY(write_bus(j,j->plan->recipe.unlock0,j->plan->recipe.erase));
  TRY(unlock(j));TRY(write_bus(j,pa,j->plan->recipe.sector));
  TRY(poll(j,pa,0xffff,timeout));
 }
 TRY(reset(j,pa));if(readback)TRY(verify(j,a,NULL,len));
done:return end(j,rc);
}
int cb_nor_erase(struct cb_nor_job*j,uint32_t a,uint32_t timeout){return erase_impl(j,a,timeout,true);}
int cb_nor_erase_poll(struct cb_nor_job*j,uint32_t a,uint32_t timeout){return erase_impl(j,a,timeout,false);}
static int program_impl(struct cb_nor_job*j,uint32_t a,const uint8_t*data,size_t len,uint32_t timeout,bool readback){
 uint32_t pa;
 unsigned unit,dies;
 int rc=guard(j);
 if(rc)return reject(j,rc);
 if(!data||!timeout||timeout>120000||!len||cb_nor_chunk(j->plan,a,len)!=len)return reject(j,CB_NOR_ARGUMENT);
 unit=j->plan->recipe.unit;dies=dual(j)?2:1;
 if(dual(j)&&((a&3)||(len&3)))return reject(j,CB_NOR_ARGUMENT);
 pa=physical(j,a);j->failed_address=a;
 rc=select_bus(j,a);if(rc)return reject(j,rc);
 TRY(reset(j,0));
 if(intel(j)){
  if(j->plan->flags&CHISBURNER_NOR_FLAG_DISABLE_BUFFER_PROGRAM){
   /* Mori's M36L0R806 profile: status at 0, 10h at 0, then PA/PD. */
   TRY(write_bus(j,0,cmd(j,0x70)));TRY(poll(j,0,0,timeout));
   TRY(write_bus(j,0,cmd(j,0x10)));
   TRY(write_bus(j,pa,(uint16_t)(data[0]|(uint16_t)data[1]<<8)));
  }else if(!j->plan->geometry.buffer_bytes){
   /* Standard Intel/Sharp word programming; no invented buffer geometry. */
   TRY(write_bus(j,pa,j->plan->recipe.word));
   TRY(write_bus(j,pa,(uint16_t)(data[0]|(uint16_t)data[1]<<8)));
  }else{
   for(unsigned d=0;d<dies;d++){
    TRY(write_bus(j,pa+d*2,j->plan->recipe.buffer));
    TRY(poll(j,pa+d*2,0,timeout));
   }
   for(unsigned d=0;d<dies;d++)TRY(write_bus(j,pa+d*2,cb_nor_count(&j->plan->recipe,(unsigned)len/unit/dies)));
   for(size_t n=0;n<len;n+=unit)TRY(write_bus(j,pa+(uint32_t)n,(uint16_t)(data[n]|(uint16_t)data[n+1]<<8)));
   for(unsigned d=0;d<dies;d++)TRY(write_bus(j,pa+d*2,j->plan->recipe.confirm));
  }
  for(unsigned d=0;d<dies;d++)TRY(poll(j,pa+d*2,0,timeout));
 }else{
  TRY(unlock(j));
  if(j->plan->geometry.buffer_bytes){
   TRY(write_bus(j,pa,j->plan->recipe.buffer));
   TRY(write_bus(j,pa,cb_nor_count(&j->plan->recipe,(unsigned)len/unit)));
  }else TRY(write_bus(j,j->plan->recipe.unlock0,j->plan->recipe.word));
  for(size_t n=0;n<len;n+=unit){
   uint16_t v=data[n];if(unit==2)v|=(uint16_t)data[n+1]<<8;
   TRY(write_bus(j,pa+(uint32_t)n,v));
  }
  if(j->plan->geometry.buffer_bytes)TRY(write_bus(j,pa,j->plan->recipe.confirm));
  uint16_t expected=data[len-unit];if(unit==2)expected|=(uint16_t)data[len-1]<<8;
  TRY(poll(j,pa+(uint32_t)len-unit,expected,timeout));
 }
 TRY(reset(j,pa));if(readback)TRY(verify(j,a,data,len));
done:return end(j,rc);
}
int cb_nor_program(struct cb_nor_job*j,uint32_t a,const uint8_t*d,size_t n,uint32_t t){return program_impl(j,a,d,n,t,true);}
int cb_nor_program_poll(struct cb_nor_job*j,uint32_t a,const uint8_t*d,size_t n,uint32_t t){return program_impl(j,a,d,n,t,false);}
int cb_nor_finish(struct cb_nor_job*j){
 int rc=guard(j);
 if(rc){
  if(j){j->authorized=false;if(j->io.release)j->io.release(j->io.ctx);}
  return rc;
 }
 uint32_t stride=j->plan->reset_stride?j->plan->reset_stride:
  (j->plan->flags&CHISBURNER_NOR_FLAG_RESET_EVERY_1MB?0x100000u:0);
 if(stride&&((stride&(stride-1))||stride<0x100000||stride>0x2000000))return reject(j,CB_NOR_ARGUMENT);
 if(stride){
  for(uint32_t a=0;a<j->plan->accessible_bytes;a+=stride){
   TRY(select_bus(j,a));TRY(reset(j,physical(j,a)));
  }
 }
 TRY(select_bus(j,0));
done:rc=end(j,rc);j->authorized=false;return rc;
}
