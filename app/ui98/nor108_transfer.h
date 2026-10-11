// Bounded dynamic NOR runner, included after the existing SHA256 helper.
#include "../../core/nor/write_policy112.h"
static uint32_t nor108_source_crc[4096];
static int cancel108(void){of_input_poll_p0();return (_of_input_p0.buttons&OF_BTN_B)!=0;}
static uint32_t crc108(const uint8_t*p,unsigned n){
 uint32_t crc=~0u;for(unsigned i=0;i<n;i++)crc=crc32_table57[(crc^p[i])&255]^(crc>>8);return ~crc;
}
static void progress108(const char*phase,uint32_t done,uint32_t total){
 static const char *painted_phase;
 static uint32_t painted_ms;
 uint32_t tick=now();
 ui->phase=phase;ui->done=done;ui->total=total;
 ui->elapsed_ms=tick-began;ui->phase_ms=tick-phase_began;
 /* present() waits for VSync. Per-buffer rendering caps 512-byte writes
  * at roughly 30 KiB/s. Polling/cancellation remain unthrottled. */
 if(!painted_phase||strcmp(painted_phase,phase)||!done||done==total||
    (uint32_t)(tick-painted_ms)>=100u){
  paint();painted_phase=phase;painted_ms=now();
 }
}
static int program_nor108(void){
 const of_nor_info_t*n=&ui->analysis.nor;
 uint32_t token=0,unit=n->bus==2?2:1;
 struct cb_cart source={0};sha256_ctx_t hash;
 char expected[65],actual[65];int rc=-1,mode=-1;
 if(!of_nor_available()||!(n->flags&OF_NOR_WRITE_OK)||!image_bytes||
    !cb_nor_write_capacity112(n->bus,n->flash_bytes)||
    image_bytes>n->accessible_bytes||image_bytes>0x10000000u||image_bytes%unit||image_bytes<512)return -1;
 ui->task="Write ROM / erases covered sectors";
 snprintf(ui->path,sizeof(ui->path),CB_ROM_INPUT_PATH " / %lu bytes",(unsigned long)image_bytes);
 phase_began=now();sha256_init(&hash);
 for(uint32_t off=0;off<image_bytes;off+=sizeof(buffer)){
  unsigned amount=image_bytes-off;if(amount>sizeof(buffer))amount=sizeof(buffer);
  progress108("Scan ROM file",off,image_bytes);
  if(cancel108())return -3;
  rc=source_read(off,buffer,amount);if(rc)return rc;
  if(!off){
   if(n->bus==2?!cb_parse_gba(&source,buffer):!cb_parse_gb(&source,buffer+0x100))return -7;
   snprintf(ui->source_title,sizeof(ui->source_title),"%s",source.title);
   if(n->bus==1){
    mode=cb_dump_mode(&source);
    if(source.rom_bytes!=image_bytes||mode<3||mode>5||
       (n->mapper==5?mode!=5:mode==5))return -2;
   }
  }
  nor108_source_crc[off/sizeof(buffer)]=crc108(buffer,amount);sha256_update(&hash,buffer,amount);
 }
 endhash106(&hash,expected);snprintf(ui->sha256,sizeof(ui->sha256),"%s",expected);
 ui->notice="Last covered sector tail is erased";
 if(!confirm())return -3;
 progress108("Re-identify NOR before erase",0,image_bytes);
 rc=of_nor_begin(n->generation,image_bytes,&token);if(rc)return rc;
 phase_began=now();
 for(uint32_t address=0;address<image_bytes;){
  uint32_t erased=0;progress108("Erase + blank-check sector",address,image_bytes);
  if(cancel108()){rc=-3;goto failed;}
  rc=of_nor_erase(token,address,&erased);if(rc)goto failed;
  if(!erased||erased>n->accessible_bytes-address){rc=-1;goto failed;}address+=erased;
 }
 phase_began=now();sha256_init(&hash);
 for(uint32_t off=0;off<image_bytes;off+=sizeof(buffer)){
  unsigned amount=image_bytes-off;if(amount>sizeof(buffer))amount=sizeof(buffer);
  progress108("Program + readback verify",off,image_bytes);
  if(cancel108()){rc=-3;goto failed;}
  rc=source_read(off,buffer,amount);if(rc)goto failed;
  if(crc108(buffer,amount)!=nor108_source_crc[off/sizeof(buffer)]){rc=-7;goto failed;}
  sha256_update(&hash,buffer,amount);
  for(unsigned b=0;b<amount;){
   if(cancel108()){rc=-3;goto failed;}
   uint32_t address=off+b,limit=n->buffer_bytes?n->buffer_bytes:unit;
   unsigned length=limit-address%limit;if(length>amount-b)length=amount-b;
   unsigned window=n->bus==2?0x2000000:0x4000;
   if(length>window-address%window)length=window-address%window;
   for(unsigned r=0;r<n->region_count;r++){
    const of_nor_region_t*g=&n->regions[r];uint32_t span=g->count*g->sector_bytes;
    if(address>=g->start&&address-g->start<span){
     uint32_t remain=g->sector_bytes-(address-g->start)%g->sector_bytes;
     if(length>remain)length=remain;
     break;
    }
   }
   if(!length||length%unit){rc=-1;goto failed;}
   rc=of_nor_program(token,address,buffer+b,length);if(rc)goto failed;b+=length;
  }
 }
 endhash106(&hash,actual);if(strcmp(expected,actual)){rc=-7;goto failed;}
 int route_verify=of_nor_verify_available();
 if(!route_verify){
  /* Older OS: only the already bounded legacy-window plan is allowed. */
  if(n->bus==2&&image_bytes>0x2000000u){rc=-2;goto failed;}
  rc=of_nor_finish(token);token=0;if(rc)return rc;
  rc=of_cart_enter_safe_idle();if(rc)return rc;
 }
 // Read the entire image again, not just status/previous chunk checks.
 phase_began=now();sha256_init(&hash);
 for(uint32_t off=0;off<image_bytes;off+=sizeof(buffer)){
  unsigned amount=image_bytes-off;if(amount>sizeof(buffer))amount=sizeof(buffer);
  progress108("Full cartridge SHA256 verify",off,image_bytes);
  if(cancel108()){rc=-3;goto failed;}
  if(route_verify){
   for(unsigned b=0;b<amount;b+=1024){
    unsigned length=amount-b;if(length>1024)length=1024;
    rc=of_nor_verify_read(token,off+b,buffer+b,length);
    if(rc)goto failed;
   }
  }else for(unsigned b=0;b<amount;b+=32){
   uint8_t tail[32];unsigned length=amount-b;if(length>32)length=32;
   if(n->bus==2)rc=of_cart_read(OF_CART_BUS_GBA,off+b,tail,32,50000);
   else rc=of_cart_read_space(OF_CART_BUS_GB_GBC,OF_CART_SPACE_IDENTIFY,0x10000000u+((unsigned)mode<<24)+off+b,tail,32,50000);
   if(rc)goto failed;
   memcpy(buffer+b,tail,length);
  }
  sha256_update(&hash,buffer,amount);
 }
 endhash106(&hash,actual);snprintf(ui->sha256,sizeof(ui->sha256),"%s",actual);
 if(strcmp(expected,actual)){rc=-7;goto failed;}
 if(route_verify){rc=of_nor_finish(token);token=0;if(rc)return rc;}
 progress108("Complete / full SHA256 matched",image_bytes,image_bytes);return 0;
failed:
 if(token)(void)of_nor_finish(0);
 return rc;
}
