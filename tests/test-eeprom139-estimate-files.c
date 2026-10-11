/* Offline replay only: no cartridge operations, input files opened read-only. */
#include "../core/save/eeprom139.h"
#include <assert.h>
#include <stdio.h>
static unsigned char small[512],large[8192];
static void read_exact(const char *path,unsigned char *p,unsigned n){
 FILE *f=fopen(path,"rb");assert(f);assert(fread(p,1,n,f)==n);
 assert(fgetc(f)==EOF);assert(!ferror(f));fclose(f);
}
int main(int argc,char **argv){
 assert(argc==4);
 read_exact(argv[1],small,512);read_exact(argv[2],large,8192);
 struct cb_eeprom139_report r={.stable_mask=3};
 assert(cb_eeprom139_estimate(&r,small,large)==CB_EEPROM139_512);
 read_exact(argv[3],large,8192);r.stable_mask=2;r.mismatch_mask=1;
 assert(cb_eeprom139_estimate(&r,small,large)==CB_EEPROM139_8K);
 puts("PASS offline captures: dual-stable -> 512 B; large-only stable -> 8 KiB estimate (not new hardware validation)");
}
