#include <save/flash170.h>
/* All input, snapshots and reports are SDRAM. No generic write service. */
static int flash170_delay(void *ctx,unsigned us){
 (void)ctx;if(us>10000)return CB_NOR_ARGUMENT;
 uint64_t end=read_cycles()+(uint64_t)us*(CPU_FREQ_HZ/1000000u);
 do{if(!alive(NULL))return CB_NOR_CANCELLED;}while(read_cycles()<end);
 return 0;
}
int flash170_restore_service(const void *source,void *disk_backup,uint32_t bytes,
 const uint8_t expected_id[16],void *out,uint32_t out_size){
 struct cb_flash170_report r={0};uint8_t id[16];
 if(out_size!=sizeof r||(bytes!=65536u&&bytes!=131072u)||
    !save158_sdram_range(source,bytes)||!save158_sdram_range(disk_backup,bytes)||
    !save158_sdram_range(out,out_size)||!save158_sdram_range(expected_id,16)||
    save158_overlaps(source,bytes,disk_backup,bytes)||
    save158_overlaps(source,bytes,out,out_size)||save158_overlaps(disk_backup,bytes,out,out_size))return CB_NOR_ARGUMENT;
 memcpy(id,expected_id,16);
 if(of_file_size(5u)!=(long)bytes)return CB_NOR_ARGUMENT;
 for(unsigned off=0;off<bytes;off+=32768){
  r.rc=of_file_read(5u,off,(uint8_t*)disk_backup+off,32768);if(r.rc)goto finish;
 }
 if(busy){r.rc=CB_NOR_IO;goto finish;}
 busy=true;job.authorized=false;job_token=0;mapper_kind=0;
 r.rc=enter(2);
 if(!r.rc&&(CART_CAPS&0x000100bfu)!=0x000100bfu)r.rc=CB_NOR_UNSUPPORTED;
 if(!r.rc)r.rc=eeprom162_header(id);
 if(!r.rc){
  const struct cb_save139_flash_ops probe={{NULL,save139_read,save139_write,save139_alive},save139_delay};
  struct cb_save139_flash_report p=cb_save139_probe_flash(&probe);
  r.id=p.id;r.rc=p.cleanup_rc?p.cleanup_rc:p.rc;
  if(!r.rc&&(!p.exit_verified||!p.id_read||cb_flash170_size(p.id)!=bytes))r.rc=CB_NOR_UNSUPPORTED;
  if(!r.rc){
   const struct cb_flash170_ops ops={NULL,save139_read,save158_write,flash170_delay,now_ms};
   r.rc=cb_flash170_run(&ops,p.id,source,disk_backup,bytes,&r);
  }
 }
 {int clean=private_owned?leave():0;busy=false;
  if(clean&&!r.cleanup_rc)r.cleanup_rc=clean;
  if(!r.rc)r.rc=r.cleanup_rc;}
finish:
 memcpy(out,&r,sizeof r);return r.rc;
}
