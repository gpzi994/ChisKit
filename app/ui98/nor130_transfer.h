// SPDX-License-Identifier: AGPL-3.0-only
// Serial TF -> dual PSRAM -> NOR pipeline; final whole-accessible-NOR compare.
static uint8_t verify130[1024] __attribute__((aligned(64)));
static int program_nor130(void){
 const of_nor_info_t*n=&ui->analysis.nor;struct cb_cart source={0};sha256_ctx_t hash;
 char expected[65],actual[65];uint32_t token=0,unit=n->bus==2?2:1;int rc=-1,mode=-1;
 if(!of_nor_pipeline_available()||!(n->flags&OF_NOR_WRITE_OK)||!image_bytes||
    !cb_nor_write_capacity112(n->bus,n->flash_bytes)||n->accessible_bytes>0x02000000u||
    image_bytes>n->accessible_bytes||image_bytes%unit||image_bytes<512u)return -1;
 ui->task="Write ROM / PSRAM staged";
 snprintf(ui->path,sizeof(ui->path),CB_ROM_INPUT_PATH " / %lu bytes",(unsigned long)image_bytes);
 phase_began=now();sha256_init(&hash);
 for(uint32_t off=0;off<image_bytes;off+=sizeof(buffer)){
  uint32_t amount=image_bytes-off;if(amount>sizeof(buffer))amount=sizeof(buffer);
  progress108("TF -> PSRAM cache",off,image_bytes);if(cancel108())return -3;
  for(uint32_t b=0;b<amount;b+=4096u){
   uint32_t length=amount-b;if(length>4096u)length=4096u;
   rc=of_psram_cache_load(CB_ROM_INPUT_SLOT,off+b,off+b,length);if(rc)return rc;
  }
  rc=of_psram_cache_read(off,buffer,amount);if(rc)return rc;
  if(!off){
   if(n->bus==2?!cb_parse_gba(&source,buffer):!cb_parse_gb(&source,buffer+0x100))return -7;
   snprintf(ui->source_title,sizeof(ui->source_title),"%s",source.title);
   if(n->bus==1){mode=cb_dump_mode(&source);if(source.rom_bytes!=image_bytes||mode<3||mode>5||
     (n->mapper==5?mode!=5:mode==5))return -2;}
  }
  sha256_update(&hash,buffer,amount);
 }
 endhash106(&hash,expected);snprintf(ui->sha256,sizeof(ui->sha256),"%s",expected);
 ui->notice="Full erase; one final whole-NOR verify";
 if(!confirm())return -3;
 progress108("Re-identify NOR before erase",0,n->accessible_bytes);
 rc=of_nor_begin_full(n->generation,image_bytes,n->accessible_bytes,&token);if(rc)return rc;
 phase_began=now();
 for(uint32_t address=0;address<n->accessible_bytes;){
  uint32_t erased=0;progress108("Erase sector / status poll",address,n->accessible_bytes);
  if(cancel108()){rc=-3;goto failed;}
  rc=of_nor_erase_poll(token,address,&erased);if(rc)goto failed;
  if(!erased||erased>n->accessible_bytes-address){rc=-1;goto failed;}address+=erased;
 }
 phase_began=now();
 if(of_nor_stream_available()){
  for(uint32_t off=0;off<image_bytes;){
   uint32_t amount=0;
   progress108("PSRAM -> NOR / hardware",off,image_bytes);
   if(cancel108()){rc=-38;goto failed;}
   rc=of_nor_program_next(token,off,image_bytes-off,&amount);
   if(rc!=1){if(!rc)rc=-5;goto failed;}
   int stopping=0;
   do {
    if(cancel108())stopping=1;
    rc=of_nor_program_step(token,stopping);
   }while(rc==1); // Never leave the page with a live transaction/lease.
   if(rc)goto failed;
   if(!amount || amount>image_bytes-off){rc=-5;goto failed;}
   off+=amount;
  }
 }else{
 for(uint32_t off=0;off<image_bytes;off+=sizeof(buffer)){
  uint32_t amount=image_bytes-off;if(amount>sizeof(buffer))amount=sizeof(buffer);
  progress108("PSRAM -> NOR / status poll",off,image_bytes);if(cancel108()){rc=-3;goto failed;}
  rc=of_psram_cache_read(off,buffer,amount);if(rc)goto failed;
  for(uint32_t b=0;b<amount;){
   uint32_t address=off+b,limit=n->buffer_bytes?n->buffer_bytes:unit;
   uint32_t length=limit-address%limit;if(length>amount-b)length=amount-b;
   uint32_t window=n->bus==2?0x2000000u:0x4000u;
   if(length>window-address%window)length=window-address%window;
   for(unsigned r=0;r<n->region_count;r++){
    const of_nor_region_t*g=&n->regions[r];uint32_t span=g->count*g->sector_bytes;
    if(address>=g->start&&address-g->start<span){uint32_t remain=g->sector_bytes-(address-g->start)%g->sector_bytes;
     if(length>remain)length=remain;
     break;}
   }
   if(!length||length%unit){rc=-1;goto failed;}
   rc=of_nor_program_poll(token,address,buffer+b,length);if(rc)goto failed;b+=length;
  }
 }
 }
 if((n->bus==2 || n->bus==1) && of_nor_compare_available()){
  phase_began=now(); // Hardware compare rate excludes the preceding programming time.
  uint32_t off=0;
  while(off<n->accessible_bytes){
   uint32_t amount=n->accessible_bytes-off;
   uint32_t compare_chunk=n->bus==1?16384u:65536u;
   if(amount>compare_chunk)amount=compare_chunk;
   progress108("Whole-NOR compare / hardware",off,n->accessible_bytes);
   if(cancel108()){rc=-3;goto failed;}
   rc=of_nor_compare_start(token,off,amount);
   if(rc==-2 && !off)break; // Unsupported board: preserve complete legacy verification.
   if(rc!=1){if(!rc)rc=-5;goto failed;}
   int stopping=0;
   do{if(cancel108())stopping=1;rc=of_nor_program_step(token,stopping);}while(rc==1);
   if(rc){
    uint32_t diagnostic[4];
    if(!of_nor_compare_result(token,diagnostic) && ((diagnostic[0]>>4)&15u)==6u){
     snprintf(ui->path,sizeof(ui->path),"At %08lx exp %04lx got %04lx",
      (unsigned long)diagnostic[2],(unsigned long)(diagnostic[3]&65535u),
      (unsigned long)(diagnostic[3]>>16));
     ui->notice=ui->path;
    }
    goto failed;
   }
   off+=amount;
  }
  if(off==n->accessible_bytes)goto verified130;
 }
 phase_began=now();sha256_init(&hash);
 for(uint32_t off=0;off<n->accessible_bytes;off+=1024u){
  uint32_t amount=n->accessible_bytes-off;if(amount>1024u)amount=1024u;
  progress108("Final whole-NOR compare",off,n->accessible_bytes);if(cancel108()){rc=-3;goto failed;}
  rc=of_nor_verify_full_read(token,off,buffer,amount);if(rc)goto failed;
  if(off<image_bytes){
   uint32_t expected_len=image_bytes-off;if(expected_len>amount)expected_len=amount;
   rc=of_psram_cache_read(off,verify130,expected_len);if(rc)goto failed;
   if(memcmp(buffer,verify130,expected_len)){rc=-7;goto failed;}
   sha256_update(&hash,buffer,expected_len);
   for(uint32_t b=expected_len;b<amount;b++)if(buffer[b]!=0xff){rc=-7;goto failed;}
  }else for(uint32_t b=0;b<amount;b++)if(buffer[b]!=0xff){rc=-7;goto failed;}
 }
 endhash106(&hash,actual);snprintf(ui->sha256,sizeof(ui->sha256),"%s",actual);
 if(strcmp(expected,actual)){rc=-7;goto failed;}
verified130:
 rc=of_nor_finish(token);token=0;if(rc)return rc;
 progress108("PASS / whole NOR matched",n->accessible_bytes,n->accessible_bytes);return 0;
failed:
 if(rc==-20)ui->notice="Hardware authorization ACK failed";
 else if(rc==-21)ui->notice="Hardware submit ACK failed";
 else if(rc<=-22&&rc>=-38){
  snprintf(ui->io_diag,sizeof(ui->io_diag),"Hardware guard failed: %d",rc);
  ui->notice=ui->io_diag;
 }
 if(token)(void)of_nor_finish(token);
 return rc;
}
