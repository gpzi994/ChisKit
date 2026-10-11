#ifndef CB_SETTINGS174_H
#define CB_SETTINGS174_H
#include <stdio.h>
#include <string.h>
#ifdef CB_LANG174
#include "of_output.h"
int cb_settings_read198(void *dst);
#endif
/* Production uses an app-owned dynamic file, not an APF nonvolatile slot.
 * The legacy branch retains its shared-slot preservation policy. */
static int cb_settings174_decode(const unsigned char b[16]){
 return !memcmp(b,"CBLANG174:",10)&&(b[10]>='0'&&b[10]<='3')&&
  b[11]=='\n'&&!memcmp(b+12,"END\n",4)?b[10]-'0':-1;
}
static unsigned cb_settings174_load(void){
#ifdef CB_LANG174
 if(of_output_open("/Saves/chiskit/common/chiskit-language.cfg",16)<0)return 0;
 unsigned char b[16]={0};
 int v=cb_settings_read198(b)?-1:cb_settings174_decode(b);
 return v<0?0:(unsigned)v;
#else
 unsigned char b[16]={0};FILE *f=fopen("slot:8","rb");if(!f)return 0;
 size_t n=fread(b,1,16,f);int closed=fclose(f);
 int v=n==16&&!closed?cb_settings174_decode(b):-1;return v<0?0:(unsigned)v;
#endif
}
static int cb_settings174_save(unsigned language){
 if(language>3)return -1;
#ifdef CB_LANG174
 unsigned char b[16]="CBLANG174:0\nEND\n",verify[16];
 int rc=of_output_open("/Saves/chiskit/common/chiskit-language.cfg",16);
 if(rc<0)return -2;
 b[10]=(unsigned char)('0'+language);
 rc=of_output_write(0,b,16);if(rc)return -6;
 /* Pocket does not ACK target FLUSH (0x0188). Match the verified dump
  * contract: synchronous write completion followed by full readback. */
 if(cb_settings_read198(verify))return -9;
 return memcmp(b,verify,16)?-11:0;
#else
 unsigned char old[16]={0},b[16]="CBLANG174:0\nEND\n",verify[16];
 FILE *f=fopen("slot:8","r+b");if(!f)return -2;
 size_t n=fread(old,1,16,f);int empty=1;
 for(size_t i=0;i<n;i++)if(old[i]!=0&&old[i]!=255)empty=0;
 if(ferror(f)){fclose(f);return -3;}
 if(!empty&&(n!=16||cb_settings174_decode(old)<0)){fclose(f);return -4;}
 b[10]=(unsigned char)('0'+language);
 int rc=fseek(f,0,SEEK_SET)?-5:0;
 if(!rc&&fwrite(b,1,16,f)!=16)rc=-6;
 if(fclose(f)&&!rc)rc=-7;
 if(rc)return rc;
 f=fopen("slot:8","rb");if(!f)return -8;
 n=fread(verify,1,16,f);rc=fclose(f);
 if(n!=16)return -9;
 if(rc)return -10;
 return !memcmp(b,verify,16)?0:-11;
#endif
}
#endif
