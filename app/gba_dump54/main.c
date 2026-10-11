#include <stdio.h>
#include <unistd.h>
#include "of_output.h"
#include "dump.h"
#include "read_block.h"
static struct guard51 gate;
static struct scan54 boot;
static unsigned reads,writes,cart_reads,binds;
static of_cart_caps_t caps;
static const char *phase54(enum stage54 p){
    static const char *n[]={"BOOT TF SCAN","CART CAPABILITY","CART ID SAMPLES","TF BIND","CART ROM -> TF","TF WRITE","BLOCK READBACK","FULL TF MD5","COMPLETE"};
    return n[p];
}
static void title54(void){printf("\033[2J\033[H GBA DUMP smoke.54\nCHISFLASH ROM READ ONLY\n");}
static void hash54(const uint8_t *d){char h[33];md5_hex53(d,h);printf("%s\n",h);}
static void progress54(enum stage54 phase,unsigned blocks){
    title54();printf("%s\nBLOCKS %u/%u  TF CHUNK 32 KiB\n",phase54(phase),blocks,BLOCKS54);
    printf("FILE dump54.gba\nSIZE 8705280 BYTES\nCART READ32 %u/272045\n",cart_reads);
    printf("TF WRITE %u READ %u BIND %u\n",writes,reads,binds);
    printf("FLUSH: NOT USED\nSAVE MEMORY: NOT ACCESSED\nPLEASE WAIT; KEEP POWER ON\n");fflush(stdout);
}
static void show_boot54(void){
    title54();printf("BOOT CONTENT: %s\n",boot.kind==ZERO54?"ZERO SEED":boot.kind==ROM54?"ROM VERIFIED":"UNKNOWN");
    printf("TF RC %d BYTES %u/8705280\n",boot.rc,boot.bytes);
    printf("%sMD5\n",boot.bytes==TOTAL54?"":"PARTIAL ");hash54(boot.md5);
    printf("TF WRITE %u CART READ %u\n",writes,cart_reads);
    printf("BOOT SCAN MS %lu\n",(unsigned long)boot.ms);
    if(boot.kind!=ZERO54 || boot.rc){
        printf("WRITING: DISABLED\n");
        if(boot.kind==ROM54)printf("FULL ROM MD5 MATCH\nSEND THIS SCREEN; NO REWRITE\n");
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
        printf("ONLY dump54.gba MAY BE WRITTEN\nCARTRIDGE ROM/SAVE NEVER WRITTEN\n");
    }
    printf("FLUSH: NOT USED\n");fflush(stdout);
}
static void result54(const struct job54 *r){
    title54();printf("ROM DUMP %s RC %d\n",r->rc?"FAIL":"PASS",r->rc);
    printf("STAGE %s\n",phase54(r->stage));
    printf("BYTES %u/8705280 BLOCKS %u/%u\n",r->bytes,r->blocks,BLOCKS54);
    printf("SAMPLES %u/5 SAFE RC %d\n",r->samples,r->safe_rc);
    if(r->stop==TOTAL54)printf("STOP OFFSET: NONE\n");else printf("STOP OFFSET %08lX\n",(unsigned long)r->stop);
    printf("CART MD5 (FULL PASS ONLY)\n");hash54(r->cart_md5);
    printf("TF MD5 (%u BYTES)\n",r->verify.bytes);hash54(r->verify.md5);
    printf("SECONDS %lu; FLUSH NOT USED\n",(unsigned long)(r->ms/1000));
    if(!r->rc)printf("QUIT; POWER OFF/ON; PLAY CARTRIDGE\nEXPECT BOOT CONTENT: ROM VERIFIED\nDO NOT RECOPY dump54.gba\n");
    else printf("STOP; SEND SCREEN; NO RETRY\nPARTIAL OUTPUT FILE MAY REMAIN\n");
    printf("LEFT/RIGHT: TIMING PAGE 2\n");fflush(stdout);
}
static void timing54(const struct job54 *r){
    const struct times54 *t=&r->time;uint32_t accounted=accounted54(t);
    title54();printf("TIMING PAGE 2/2 - MILLISECONDS\n");
    printf("CART READ      %lu\n",(unsigned long)t->cart);
    printf("TF WRITE       %lu\n",(unsigned long)t->write);
    printf("BLOCK READBACK %lu\n",(unsigned long)t->readback);
    printf("HASH+COMPARE   %lu\n",(unsigned long)t->hash);
    printf("SAFE IDLE      %lu\n",(unsigned long)t->safe);
    printf("FINAL TF MD5   %lu\n",(unsigned long)t->verify);
    printf("OTHER / UI     %lu\n",(unsigned long)(r->ms>=accounted?r->ms-accounted:0));
    printf("DUMP TOTAL     %lu\n",(unsigned long)r->ms);
    printf("BOOT (OUTSIDE) %lu\n",(unsigned long)boot.ms);
    printf("TF W %u R %u CART %u\n",writes,reads,cart_reads);
    printf("TF 32 KiB; CART 32 B UNCHANGED\n");
    printf("TIMING SUM %s; RC %d\n",r->ms>=accounted?"OK":"BAD",r->rc);
    printf("FLUSH NOT USED; SAVE NOT ACCESSED\n");
    printf("LEFT/RIGHT OR A: RESULT PAGE 1\n");fflush(stdout);
}
#ifndef OF_PC
#include "of_file.h"
#include "of_input.h"
static uint32_t now54(void){return OF_SVC->timer_get_ms();}
static void idle54(void){usleep(1000);}
static int sdk_read54(uint32_t off,uint8_t *dst,uint32_t n){
    const struct read_ops54 o={of_file_read_async,of_file_async_poll,of_file_async_busy,now54,idle54};
    ++reads;return read_block54(&o,off,dst,n);
}
static int sdk_write54(uint32_t off,const void *src,uint32_t n){++writes;return of_output_write(off,src,n);}
static int sdk_cart54(uint32_t off,uint8_t *dst){
    ++cart_reads;return of_cart_read_space(OF_CART_BUS_GBA,OF_CART_SPACE_ROM,off,dst,32,50000);
}
static int sdk_bind54(void){++binds;return of_output_open(NAME54,TOTAL54);}
static void halt54(void){fflush(stdout);while(1)usleep(100000);}
#endif
int main(int argc,char **argv){
    (void)fill51;(void)identify51;(void)crc51;
#ifdef OF_PC
    (void)argc;(void)argv;(void)run54;(void)gate_tick52;(void)read_block54;
    boot=(struct scan54){.kind=ZERO54,.bytes=TOTAL54,.blocks=BLOCKS54};caps.lifecycle_status=0x80000303;
    for(unsigned i=G_QUIET;i<=G_DONE;++i){gate.state=i;show_boot54();}
    boot.kind=ROM54;memcpy(boot.md5,expected54,16);show_boot54();
    boot.kind=UNKNOWN54;boot.rc=-12;boot.bytes=1280;show_boot54();
    struct job54 r={.stage=DONE54,.bytes=TOTAL54,.blocks=BLOCKS54,.samples=5,.stop=TOTAL54,.ms=900000};
    r.time=(struct times54){272045,2660,5320,17,269,8000};
    writes=266;reads=798;cart_reads=272045;
    r.verify=(struct scan54){.bytes=TOTAL54,.kind=ROM54};memcpy(r.cart_md5,expected54,16);memcpy(r.verify.md5,expected54,16);result54(&r);timing54(&r);
    r.rc=-2;r.stage=CART54;r.stop=8705248;result54(&r);timing54(&r);
    for(unsigned i=BOOT54;i<=DONE54;++i)progress54(i,BLOCKS54);
    return 0;
#else
    (void)argc;(void)argv;title54();
    if(!OF_OUTPUT_SVC_HAS(output_open) || !OF_OUTPUT_SVC_HAS(output_write) ||
       !OF_OUTPUT_SVC_HAS(file_size) || !OF_OUTPUT_SVC_HAS(timer_get_ms) || !OF_OUTPUT_SVC_HAS(input_poll_p0)){
        printf("STOP API MISSING\n");halt54();
    }
    int q=of_output_open(NULL,0);
    if(q!=MAGIC54){printf("STOP WRONG OS %08lX\nCOPY WHOLE PACKAGE; POWER OFF/ON\n",(unsigned long)q);halt54();}
    long size=OF_SVC->file_size("slot:4");
    if(size!=TOTAL54){printf("STOP SLOT4 SIZE %ld\nEXPECTED 8705280\n",size);halt54();}
    const struct ops54 o={of_cart_get_caps,sdk_cart54,of_cart_enter_safe_idle,of_cart_abort,sdk_bind54,
        sdk_write54,sdk_read54,now54,progress54};
    boot=scan54(&o,BOOT54);
    if(boot.kind!=ZERO54 || boot.rc){show_boot54();halt54();}
    q=of_cart_get_caps(&caps);
    if(q || !caps_valid54(&caps)){
        show_boot54();printf("STOP CART CAPS RC %d\nUSE PLAY CARTRIDGE; GBA INSERTED\n",q);halt54();
    }
    q=of_cart_enter_safe_idle();if(q){printf("STOP SAFE IDLE RC %d\n",q);halt54();}
    show_boot54();
    while(1){of_input_poll_p0();enum gate51 old=gate.state;
        if(gate_tick52(&gate,_of_input_p0.buttons,now54()))break;
        if(old!=gate.state)show_boot54();
        usleep(16000);
    }
    struct job54 r=run54(&gate,&boot,&o);result54(&r);
    unsigned page=0,previous=~0u;
    while(1){
        of_input_poll_p0();unsigned held=_of_input_p0.buttons,edge=held&~previous;previous=held;
        if(edge&(OF_BTN_LEFT|OF_BTN_RIGHT|OF_BTN_A)){
            if(edge&OF_BTN_A)page=0;else page^=1;
            if(page)timing54(&r);else result54(&r);
        }
        usleep(16000);
    }
#endif
}
