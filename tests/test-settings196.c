#include <assert.h>
#include <stdio.h>
#include <string.h>
static unsigned char disk[16];
/* Any reintroduced unsupported FLUSH is a compile-time failure. */
#define of_output_flush(...) POCKET_TARGET_FLUSH_IS_UNSUPPORTED
static int exists, fail_open, fail_write;
static int of_output_open(const char *p,unsigned n){
 assert(!strcmp(p,"/Saves/chiskit/common/chiskit-language.cfg")&&n==16);
 if(fail_open)return -1;
 int reused=exists;exists=1;return reused;
}
static int of_output_write(unsigned offset,const void *p,unsigned n){
 assert(offset==0&&n==16);if(fail_write)return -1;memcpy(disk,p,16);return 0;
}
int cb_settings_read198(void *dst){memcpy(dst,disk,16);return 0;}
#define OF_OUTPUT_H
#define CB_LANG174 1
#define fopen(...) CACHED_SLOT_READ_IS_FORBIDDEN
#include "../app/ui98/settings174.h"
#undef fopen
int main(void){
 assert(cb_settings174_load()==0);
 assert(cb_settings174_save(1)==0&&cb_settings174_load()==1);
 assert(cb_settings174_save(1)==0&&cb_settings174_load()==1);
 assert(cb_settings174_save(0)==0&&cb_settings174_load()==0);
 assert(cb_settings174_save(3)==0&&cb_settings174_load()==3);
 assert(cb_settings174_save(2)==0&&cb_settings174_load()==2);
 assert(cb_settings174_save(4)==-1);
 assert(cb_settings174_save(0)==0);
 fail_open=1;assert(cb_settings174_save(1)==-2);fail_open=0;
 fail_write=1;assert(cb_settings174_save(1)==-6);fail_write=0;
 assert(!memcmp(disk,"CBLANG174:0\nEND\n",16));
 puts("settings196 PASS: slot5 explicit IO, reuse +1, roundtrip, defaults, errors");
}
