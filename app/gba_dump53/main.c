#include <stdio.h>
#include <unistd.h>
#include "of_output.h"
#include "dump.h"
#include "read_block.h"
static struct guard51 gate;
static struct scan53 boot;
static unsigned reads,writes,cart_reads,binds;
static of_cart_caps_t caps;
static const char *phase53(enum stage53 p){
    static const char *n[]={"BOOT TF SCAN","CART CAPABILITY","CART ID SAMPLES","TF BIND","CART ROM -> TF","TF WRITE","BLOCK READBACK","FULL TF MD5","COMPLETE"};
    return n[p];
}
static void title53(void){printf("\033[2J\033[H GBA DUMP smoke.53\nCHISFLASH ROM READ ONLY\n");}
static void hash53(const uint8_t *d){char h[33];md5_hex53(d,h);printf("%s\n",h);}
static void progress53(enum stage53 phase,unsigned blocks){
    title53();printf("%s\nBLOCKS %u/2126\n",phase53(phase),blocks);
    printf("FILE dump53.gba\nSIZE 8705280 BYTES\nCART READ32 %u/272045\n",cart_reads);
    printf("TF WRITE %u READ %u BIND %u\n",writes,reads,binds);
    printf("FLUSH: NOT USED\nSAVE MEMORY: NOT ACCESSED\nPLEASE WAIT; KEEP POWER ON\n");fflush(stdout);
}
static void show_boot53(void){
    title53();printf("BOOT CONTENT: %s\n",boot.kind==ZERO53?"ZERO SEED":boot.kind==ROM53?"ROM VERIFIED":"UNKNOWN");
    printf("TF RC %d BYTES %u/8705280\n",boot.rc,boot.bytes);
    printf("%sMD5\n",boot.bytes==TOTAL53?"":"PARTIAL ");hash53(boot.md5);
    printf("TF WRITE %u CART READ %u\n",writes,cart_reads);
    if(boot.kind!=ZERO53 || boot.rc){
        printf("WRITE LOCK: DISABLED\n");
        if(boot.kind==ROM53)printf("FULL ROM MD5 MATCH\nSEND THIS SCREEN; NO REWRITE\n");
        else printf("STOP; SEND THIS SCREEN\nUNKNOWN FILE WILL NOT BE OVERWRITTEN\n");
    }else{
        printf("CART LIFE %08lX\n",(unsigned long)caps.lifecycle_status);
        switch(gate.state){
        case G_QUIET:printf("LOCKED: RELEASE ALL FOR 0.5 SEC\n");break;
        case G_LOCKED:printf("LOCKED: HOLD L+R FOR 2 SECONDS\n");break;
        case G_HOLD_LR:printf("KEEP L+R HELD FOR 2 SECONDS\n");break;
        case G_RELEASE_LR:printf("RELEASE ALL FOR 0.5 SECOND\n");break;
        case G_CHOOSE:printf("A: SELECT ROM DUMP; B: CANCEL\n");break;
        case G_RELEASE_MODE:printf("RELEASE ALL FOR 0.5 SECOND\n");break;
        case G_CONFIRM:printf("HOLD START (+) 1 SEC TO CONFIRM\n");break;
        case G_HOLD_START:printf("KEEP START (+) HELD 1 SECOND\n");break;
        case G_DONE:printf("CONFIRMED; ONE SHOT ONLY\n");break;
        }
        printf("ONLY dump53.gba MAY BE WRITTEN\nCARTRIDGE ROM/SAVE NEVER WRITTEN\n");
    }
    printf("FLUSH: NOT USED\n");fflush(stdout);
}
static void result53(const struct job53 *r){
    title53();printf("ROM DUMP %s RC %d\n",r->rc?"FAIL":"PASS",r->rc);
    printf("STAGE %s\n",phase53(r->stage));
    printf("BYTES %u/8705280 BLOCKS %u/2126\n",r->bytes,r->blocks);
    printf("SAMPLES %u/5 SAFE RC %d\n",r->samples,r->safe_rc);
    if(r->stop==TOTAL53)printf("STOP OFFSET: NONE\n");else printf("STOP OFFSET %08lX\n",(unsigned long)r->stop);
    printf("CART MD5 (FULL PASS ONLY)\n");hash53(r->cart_md5);
    printf("TF MD5 (%u BYTES)\n",r->verify.bytes);hash53(r->verify.md5);
    printf("SECONDS %lu; FLUSH NOT USED\n",(unsigned long)(r->ms/1000));
    if(!r->rc)printf("QUIT; POWER OFF/ON; PLAY CARTRIDGE\nEXPECT BOOT CONTENT: ROM VERIFIED\nDO NOT RECOPY dump53.gba\n");
    else printf("STOP; SEND SCREEN; NO RETRY\nPARTIAL OUTPUT FILE MAY REMAIN\n");
    printf("CARTRIDGE ROM/SAVE NEVER WRITTEN\n");fflush(stdout);
}
#ifndef OF_PC
#include "of_file.h"
#include "of_input.h"
static uint32_t now53(void){return OF_SVC->timer_get_ms();}
static void idle53(void){usleep(1000);}
static int sdk_read53(uint32_t off,uint8_t *dst,uint32_t n){
    const struct read_ops53 o={of_file_read_async,of_file_async_poll,of_file_async_busy,now53,idle53};
    ++reads;return read_block53(&o,off,dst,n);
}
static int sdk_write53(uint32_t off,const void *src,uint32_t n){++writes;return of_output_write(off,src,n);}
static int sdk_cart53(uint32_t off,uint8_t *dst){
    ++cart_reads;return of_cart_read_space(OF_CART_BUS_GBA,OF_CART_SPACE_ROM,off,dst,32,50000);
}
static int sdk_bind53(void){++binds;return of_output_open(NAME53,TOTAL53);}
static void halt53(void){fflush(stdout);while(1)usleep(100000);}
#endif
int main(int argc,char **argv){
    (void)fill51;(void)identify51;(void)crc51;
#ifdef OF_PC
    (void)argc;(void)argv;(void)run53;(void)gate_tick52;(void)read_block53;
    boot=(struct scan53){.kind=ZERO53,.bytes=TOTAL53,.blocks=BLOCKS53};caps.lifecycle_status=0x80000303;
    for(unsigned i=G_QUIET;i<=G_DONE;++i){gate.state=i;show_boot53();}
    boot.kind=ROM53;memcpy(boot.md5,expected53,16);show_boot53();
    boot.kind=UNKNOWN53;boot.rc=-12;boot.bytes=1280;show_boot53();
    struct job53 r={.stage=DONE53,.bytes=TOTAL53,.blocks=BLOCKS53,.samples=5,.stop=TOTAL53,.ms=900000};
    r.verify=(struct scan53){.bytes=TOTAL53,.kind=ROM53};memcpy(r.cart_md5,expected53,16);memcpy(r.verify.md5,expected53,16);result53(&r);
    r.rc=-2;r.stage=CART53;r.stop=8705248;result53(&r);
    for(unsigned i=BOOT53;i<=DONE53;++i)progress53(i,BLOCKS53);
    return 0;
#else
    (void)argc;(void)argv;title53();
    if(!OF_OUTPUT_SVC_HAS(output_open) || !OF_OUTPUT_SVC_HAS(output_write) ||
       !OF_OUTPUT_SVC_HAS(file_size) || !OF_OUTPUT_SVC_HAS(timer_get_ms) || !OF_OUTPUT_SVC_HAS(input_poll_p0)){
        printf("STOP API MISSING\n");halt53();
    }
    int q=of_output_open(NULL,0);
    if(q!=MAGIC53){printf("STOP WRONG OS %08lX\nCOPY WHOLE PACKAGE; POWER OFF/ON\n",(unsigned long)q);halt53();}
    long size=OF_SVC->file_size("slot:4");
    if(size!=TOTAL53){printf("STOP SLOT4 SIZE %ld\nEXPECTED 8705280\n",size);halt53();}
    const struct ops53 o={of_cart_get_caps,sdk_cart53,of_cart_enter_safe_idle,of_cart_abort,sdk_bind53,
        sdk_write53,sdk_read53,now53,progress53};
    boot=scan53(&o,BOOT53);
    if(boot.kind!=ZERO53 || boot.rc){show_boot53();halt53();}
    q=of_cart_get_caps(&caps);
    if(q || !caps_valid53(&caps)){
        show_boot53();printf("STOP CART CAPS RC %d\nUSE PLAY CARTRIDGE; GBA INSERTED\n",q);halt53();
    }
    q=of_cart_enter_safe_idle();if(q){printf("STOP SAFE IDLE RC %d\n",q);halt53();}
    show_boot53();
    while(1){of_input_poll_p0();enum gate51 old=gate.state;
        if(gate_tick52(&gate,_of_input_p0.buttons,now53()))break;
        if(old!=gate.state)show_boot53();
        usleep(16000);
    }
    struct job53 r=run53(&gate,&boot,&o);result53(&r);halt53();
#endif
}
