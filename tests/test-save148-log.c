#include "../app/ui98/save_log148.h"
#include <assert.h>
#include <string.h>
int main(void){
 char log[1024];of_save141_diagnostic d={0};d.stage=7;d.direct_rc=-3;d.original=0x4c;d.restored=0x4c;
 assert(cb_save148_format(log,sizeof log,"DRAGONBALLAA",-3,&d)>0);
 assert(strstr(log,"stage=7")&&strstr(log,"direct_rc=-3")&&strstr(log,"original=4C")&&strstr(log,"END dev.149-auto"));
 assert(cb_save148_format(log,8,"test",0,&d)==-1);
 d.stage=9;assert(cb_save148_format(log,sizeof log,"GBAZELDA MC",-3,&d)>0&&strstr(log,"stage=9"));
 puts("PASS save148 diagnostic format, stage7/stage9, truncation rejection");
}
