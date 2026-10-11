#include "of_save171.h"
/* OS .bss is SDRAM. Never accept app-supplied evidence or a raw bus command. */
static of_save162_report save171_ee;
static struct cb_save158_report save171_sram;
int save171_restore_service(uint32_t type,const void *source,void *backup,
 uint32_t bytes,const uint8_t expected_id[16],void *out,uint32_t out_size){
 if(out_size!=sizeof(of_save171_report)||
    !save158_sdram_range(out,out_size)||!save158_sdram_range(source,bytes)||
    !save158_sdram_range(backup,bytes)||!save158_sdram_range(expected_id,16)||
    !((type==1&&bytes==512)||(type==2&&bytes==8192)||
      (type==3&&bytes==32768)||(type==8&&bytes==131072))||
    save158_overlaps(source,bytes,backup,bytes)||
    save158_overlaps(source,bytes,out,out_size)||
    save158_overlaps(backup,bytes,out,out_size)||
    save158_overlaps(expected_id,16,source,bytes)||
    save158_overlaps(expected_id,16,backup,bytes)||
    save158_overlaps(expected_id,16,out,out_size))return CB_NOR_ARGUMENT;
 of_save171_report r={.version=171};
 if(busy){r.rc=CB_NOR_IO;goto done;}
 if(type==1||type==2){
  memset(&save171_ee,0,sizeof save171_ee);
  r.rc=eeprom171_restore_selected(source,backup,bytes,expected_id,
   &save171_ee,sizeof save171_ee,type);
  r.restore_rc=save171_ee.restore_rc;r.cleanup_rc=save171_ee.cleanup_rc;
  r.attempted=save171_ee.attempted;r.verified=save171_ee.verified;
  r.restore_verified=save171_ee.restore_verified;
 }else{
  memset(&save171_sram,0,sizeof save171_sram);
  r.rc=sram160_restore_impl(source,backup,bytes,expected_id,
   &save171_sram,sizeof save171_sram,1);
  r.restore_rc=save171_sram.restore_rc;r.cleanup_rc=save171_sram.bank0_rc;
  r.attempted=save171_sram.attempted;r.verified=save171_sram.verified;
  r.restore_verified=save171_sram.restore_verified?bytes:0;
 }
done:
 memcpy(out,&r,sizeof r);return r.rc;
}
