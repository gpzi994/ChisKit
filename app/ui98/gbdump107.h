/* UI107: header-sized generic ROM dump; reference hashes optional, file hash mandatory. */
#include "dump_plan.h"
#include "../gb_mbc5_fullrom86/sha256.c"
static void hash106(const uint8_t *p,unsigned n,char out[65]){
 sha256_ctx_t s;uint8_t d[32];sha256_init(&s);sha256_update(&s,p,n);sha256_final(&s,d);
 for(unsigned i=0;i<32;i++)snprintf(out+2*i,3,"%02x",d[i]);
}
static void endhash106(sha256_ctx_t *s,char out[65]){
 uint8_t d[32];sha256_final(s,d);for(unsigned i=0;i<32;i++)snprintf(out+2*i,3,"%02x",d[i]);
}
static int backup_gb107(void){
 static const char *heads[]={
  "01f91f8ffb5714cde335dd76676170dd3170a0021862dba7a0924d9599bdffa9",
  "694f303449e9241ded13453875da761a7b7fb604fc041a9450561ffe36cc77f1"};
 static const char *roms[]={
  "136ada06cb68656b7de475fa4b278d37dbeff8f5257e7dfdf7f4a4aec19a90f3",
  "8ba533be5908305a0f2f04117a858a91d4098ed8d4d2be6e48c3c3e9df1885e7"};
 unsigned total;char header[65],live[65];sha256_ctx_t hash;
 uint8_t h[96],again[96],dummy[32];int rc,which=-1;
 ui->task="Backup GB ROM / SHA256";
 ui->phase="Recheck cartridge header";paint();
 rc=of_cart_enter_safe_idle();if(rc)return rc;
 for(unsigned pass=0;pass<2;pass++){
  rc=of_cart_read(OF_CART_BUS_GB_GBC,0x100,dummy,32,50000);if(rc)return rc;
  for(unsigned off=0;off<96;off+=32){
   rc=of_cart_read(OF_CART_BUS_GB_GBC,0x100+off,(pass?again:h)+off,32,50000);if(rc)return rc;
  }
 }
 if(memcmp(h,again,96))return OF_ERR_DENIED;
 hash106(h,96,header);
 for(unsigned i=0;i<2;i++)if(!strcmp(header,heads[i]))which=(int)i;

 struct cb_cart current={0};
 if(!cb_parse_gb(&current,h)||current.type!=ui->cart.type||current.rom_bytes!=ui->cart.rom_bytes)
  return OF_ERR_DENIED;
 int mode=cb_dump_mode(&current);if(mode<0){ui->phase="Unsupported mapper / capacity";return OF_ERR_NOT_SUPPORTED;}
 total=current.rom_bytes;ui->total=total;uint32_t aperture=0x10000000u+((unsigned)mode<<24);
 snprintf(ui->source_title,sizeof(ui->source_title),"%s",current.title);
 cb_backup_name(ui->path,sizeof(ui->path),&current,"gbc",now());
 if(!confirm())return OF_ERR_DENIED;
 ui->phase="Starting sequential GB dump";ui->total=total;paint();
 rc=of_cart_enter_safe_idle();if(rc)return rc;
 /* Read the first block before file creation: an older OS/core must fail
  * without producing a misleading empty backup. Never retry mid-sequence. */
 rc=of_cart_read_space(OF_CART_BUS_GB_GBC,OF_CART_SPACE_IDENTIFY,aperture,buffer,32,50000);if(rc)return rc;
 uint8_t first[32];memcpy(first,buffer,32);
 ui->phase="Creating GB backup output";paint();
 rc=of_output_open(ui->path,total);if(rc)return rc;
 if(OF_SVC->file_size("slot:5")!=(long)total)return OF_ERR_BAD_RANGE;
 rc=backup_read(total-32,buffer,32);if(rc)return rc;
 memcpy(buffer,first,32);sha256_init(&hash);phase_began=now();
 for(unsigned off=0;off<total;off+=sizeof(buffer)){
  unsigned amount=total-off;if(amount>sizeof(buffer))amount=sizeof(buffer);
  of_input_poll_p0();if(_of_input_p0.buttons&OF_BTN_B){ui->phase="Cancelled; partial file";return OF_ERR_DENIED;}
  ui->phase="Reading GB banks";
  for(unsigned b=off?0:32;b<amount;b+=32){
   rc=of_cart_read_space(OF_CART_BUS_GB_GBC,OF_CART_SPACE_IDENTIFY,aperture+off+b,buffer+b,32,50000);if(rc)return rc;
  }
  sha256_update(&hash,buffer,amount);ui->phase="Writing GB backup output";
  rc=of_output_write(off,buffer,amount);if(rc)return rc;
  ui->done=off+amount;ui->elapsed_ms=now()-began;ui->phase_ms=now()-phase_began;paint();
 }
 endhash106(&hash,live);
 rc=of_cart_enter_safe_idle();if(rc)return rc;
 sha256_init(&hash);phase_began=now();ui->done=0;
 for(unsigned off=0;off<total;off+=sizeof(buffer)){
  unsigned amount=total-off;if(amount>sizeof(buffer))amount=sizeof(buffer);
  ui->phase="Verify file SHA256";
  rc=backup_read(off,buffer,amount);if(rc)return rc;
  sha256_update(&hash,buffer,amount);ui->done=off+amount;
  ui->elapsed_ms=now()-began;ui->phase_ms=now()-phase_began;paint();
 }
 endhash106(&hash,ui->sha256);
 if(strcmp(live,ui->sha256)){ui->phase="File differs from cart read";return OF_ERR_IO;}
 if(which>=0&&strcmp(live,roms[which])){ui->phase="ROM differs from reference";return OF_ERR_DENIED;}
 ui->notice=which<0?"File verified; no reference ROM":"File + reference SHA256 verified";
 return 0;
}
