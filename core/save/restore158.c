#include <save/restore158.h>
#include <string.h>

/* Positive internal errors stay distinct from OS transport negative codes. */
#define SAVE158_BAD_INPUT (-1)
#define SAVE158_NOT_SRAM (-2)
#define SAVE158_DIFFERENT_CART (-3)
#define SAVE158_NO_BACKUP (-4)
#define SAVE158_VERIFY_FAIL (-5)
#define SAVE158_EPOCH_LOST (-6)

static int live(const struct cb_save158_ops *o){return o->alive(o->ctx);}
static int select_bank(const struct cb_save158_ops *o,unsigned bank){
 return live(o)?o->bank(o->ctx,bank):SAVE158_EPOCH_LOST;
}
static int write_byte(const struct cb_save158_ops *o,uint32_t a,uint8_t v){
 return live(o)?o->write(o->ctx,a,v):SAVE158_EPOCH_LOST;
}
static int read_byte(const struct cb_save158_ops *o,uint32_t a,uint8_t *v){
 return live(o)?o->read(o->ctx,a,v):SAVE158_EPOCH_LOST;
}

int cb_save158_restore(const struct cb_save139_result *d,
 const uint8_t header[192],const uint8_t expected_id[16],
 const uint8_t *original,const uint8_t *disk_backup,
 const uint8_t *source,uint32_t bytes,const struct cb_save158_ops *o,
 struct cb_save158_report *r){
 if(!r)return SAVE158_BAD_INPUT;
 memset(r,0,sizeof(*r));
 if(!d||!header||!expected_id||!original||!disk_backup||!source||!o||
    !o->alive||!o->bank||!o->read||!o->write||
    (bytes!=32768u&&bytes!=131072u))return r->rc=SAVE158_BAD_INPUT;
 if(!d->backup_allowed||d->bytes!=bytes||
    !((d->type==CB_SAVE139_SRAM32&&bytes==32768u)||
      (d->type==CB_SAVE139_SRAM128&&bytes==131072u)))
  return r->rc=SAVE158_NOT_SRAM;
 if(memcmp(header+0xa0,expected_id,16))return r->rc=SAVE158_DIFFERENT_CART;
 /* The app must first write and verify a persistent SAV backup.  The fresh
  * OS-owned snapshot must match those file bytes before the first data pulse. */
 if(memcmp(original,disk_backup,bytes))return r->rc=SAVE158_NO_BACKUP;
 if(!live(o))return r->rc=SAVE158_EPOCH_LOST;
 unsigned banks=bytes==131072u?2u:1u;
 for(unsigned bank=0;bank<banks&&!r->rc;bank++){
  r->rc=select_bank(o,bank);
  for(uint32_t a=0;a<65536u&&!r->rc;a++){
   uint32_t off=bank*65536u+a;if(off>=bytes)break;
   if(original[off]==source[off])continue;
   r->attempted++;
   r->rc=write_byte(o,a,source[off]);
   if(!r->rc)r->changed++;
  }
 }
 for(unsigned bank=0;bank<banks&&!r->rc;bank++){
  r->rc=select_bank(o,bank);
  for(uint32_t a=0;a<65536u&&!r->rc;a++){
   uint32_t off=bank*65536u+a;if(off>=bytes)break;
   uint8_t value=0;r->rc=read_byte(o,a,&value);
   if(!r->rc&&value!=source[off])r->rc=SAVE158_VERIFY_FAIL;
   if(!r->rc)r->verified++;
  }
 }
 if(r->rc&&r->attempted&&live(o)){
  r->restore_attempted=1;
  for(unsigned bank=0;bank<banks&&!r->restore_rc;bank++){
   r->restore_rc=select_bank(o,bank);
   for(uint32_t a=0;a<65536u&&!r->restore_rc;a++){
    uint32_t off=bank*65536u+a;if(off>=bytes)break;
    if(original[off]==source[off])continue;
    r->restore_rc=write_byte(o,a,original[off]);
   }
  }
  for(unsigned bank=0;bank<banks&&!r->restore_rc;bank++){
   r->restore_rc=select_bank(o,bank);
   for(uint32_t a=0;a<65536u&&!r->restore_rc;a++){
    uint32_t off=bank*65536u+a;if(off>=bytes)break;
    uint8_t value=0;r->restore_rc=read_byte(o,a,&value);
    if(!r->restore_rc&&value!=original[off])r->restore_rc=SAVE158_VERIFY_FAIL;
   }
  }
  r->restore_verified=!r->restore_rc;
 }
 if(live(o))r->bank0_rc=o->bank(o->ctx,0);
 else r->bank0_rc=SAVE158_EPOCH_LOST;
 if(!r->rc)r->rc=r->bank0_rc;
 return r->rc;
}
