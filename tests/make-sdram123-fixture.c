#include "../core/sdram/fixture123.h"
#include <stdio.h>
int main(int argc,char **argv) {
 if(argc!=2) return 2;
 FILE *f=fopen(argv[1],"wb"); if(!f) return 3;
 for(uint32_t i=0;i<CB_SD123_BYTES;i++)
  if(fputc(cb_sd123_byte(i),f)==EOF){fclose(f);return 4;}
 return fclose(f)?5:0;
}
