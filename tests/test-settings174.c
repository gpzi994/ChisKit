#include <assert.h>
#include <stdio.h>
#include <string.h>
static FILE *open_mock(const char *name,const char *mode){
 assert(!strcmp(name,"slot:8"));return fopen("build/lang174-test/config.bin",mode);
}
#define fopen open_mock
#include "../app/ui98/settings174.h"
#undef fopen
int main(void){
 FILE *f=fopen("build/lang174-test/config.bin","wb");assert(f);
 unsigned char b[32]={0};b[31]=0x5a;assert(fwrite(b,1,32,f)==32);fclose(f);
 assert(cb_settings174_load()==0&&!cb_settings174_save(1)&&cb_settings174_load()==1);
 assert(!cb_settings174_save(0)&&cb_settings174_load()==0);
 f=fopen("build/lang174-test/config.bin","rb");assert(fread(b,1,32,f)==32&&b[31]==0x5a);fclose(f);
 f=fopen("build/lang174-test/config.bin","r+b");fputc('X',f);fclose(f);
 assert(cb_settings174_load()==0&&cb_settings174_save(1)<0);
 puts("LANG174 config PASS: defaults, roundtrip, preserved tail, unknown config refuses overwrite; host file mock");
}
