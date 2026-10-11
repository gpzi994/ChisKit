#include "../core/save/mori_head137.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
static unsigned char data[65536];
int main(int argc,char **argv){
 int code;struct cb_save_hint h;
 const char *sig[]={"SRAM_V","EEPROM_V","FLASH_V","FLASH512_V","FLASH1M_V"};
 const enum cb_save_kind kinds[]={CB_SAVE_SRAM,CB_SAVE_EEPROM,CB_SAVE_FLASH64,CB_SAVE_FLASH64,CB_SAVE_FLASH128};
 for(unsigned s=0;s<5;s++)for(unsigned o=0;o<65536-strlen(sig[s])+1;o+=31){
  memset(data,0,sizeof(data));memcpy(data+o,sig[s],strlen(sig[s]));
  h=cb_mori_head137(data,sizeof(data),&code);assert(h.kind==kinds[s]&&!code);
 }
 memset(data,0,sizeof(data));memcpy(data+0xac,"RZWJ",4);
 assert(cb_mori_head137(data,sizeof(data),&code).kind==CB_SAVE_SRAM&&code);
 memcpy(data+1000,"FLASH1M_V",9);
 assert(cb_mori_head137(data,sizeof(data),&code).kind==CB_SAVE_FLASH128&&!code);
 memset(data,0,sizeof(data));memcpy(data,"SRAM_V FLASH1M_V",sizeof("SRAM_V FLASH1M_V")-1);
 assert(cb_mori_head137(data,sizeof(data),&code).kind==CB_SAVE_FLASH128);
 memset(data,0,sizeof(data));memcpy(data+65530,"SRAM_V",6);
 assert(cb_mori_head137(data,sizeof(data),&code).kind==CB_SAVE_SRAM);
 assert(cb_mori_head137(data,65535,&code).kind==CB_SAVE_UNKNOWN);
 if(argc>1){
  FILE *f=fopen(argv[1],"rb");assert(f);
  struct cb_save_scan scan;cb_save_scan_init(&scan);
  size_t n;unsigned count=0;
  while((n=fread(data,1,sizeof(data),f))){
   if(!count)assert(cb_mori_head137(data,n,&code).kind==CB_SAVE_UNKNOWN);
   for(size_t i=0;i<n;i+=32)cb_save_scan_feed(&scan,data+i,n-i<32?n-i:32);
   count+=(unsigned)n;
  }
  assert(!ferror(f));fclose(f);
  h=cb_save_scan_result(&scan);assert(count==8388608&&h.kind==CB_SAVE_SRAM);
  puts("Provided Metroid ROM: old full scan SRAM; Mori 64 KiB UNKNOWN (expected)");
 }
 puts("PASS: Mori priority, all signature positions, code fallback, end bounds");
 return 0;
}
