#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "of_static_output_test.h"
#include "sequence.h"
#include "../storage_static_control/read512.h"
#include "../storage_snapshot_forensics/snapshot.h"

static struct forensic_snapshot snap;
static struct guard51 gate;
static uint8_t boot51[2][BYTES51] __attribute__((aligned(64)));
static int boot_rc51[2]={OF_ERR_INVALID_STATE,OF_ERR_INVALID_STATE};
static unsigned boot_kind,repeat_ok,eligible;
static unsigned bind_calls,write_calls,read_calls,flush_calls;
static const char *name51(unsigned p) {
    return p==INITIAL51?"INITIAL":p==A51?"A DATA":p==B51?"B DATA":"UNKNOWN";
}
static void title51(void) {
    printf("\033[2J\033[H TF GUARD smoke.51\nKEEP CARTRIDGE EMPTY\n");
}
static void classify_boot51(void) {
    boot_kind=UNKNOWN51;repeat_ok=eligible=0;
    if(boot_rc51[0] || boot_rc51[1])return;
    repeat_ok=!memcmp(boot51[0],boot51[1],BYTES51);
    boot_kind=identify51(boot51[0]);
    eligible=repeat_ok && boot_kind!=UNKNOWN51;
}
static void show_guard51(void) {
    title51();
    printf("FILE static51.bin / SLOT4 512\n");
    printf("BOOT READ %d / %d REPEAT %s\n",boot_rc51[0],boot_rc51[1],repeat_ok?"OK":"NO");
    printf("BOOT CONTENT: %s\n",name51(boot_kind));
    printf("CRC %08lX / %08lX\n",(unsigned long)crc51(boot51[0],BYTES51),(unsigned long)crc51(boot51[1],BYTES51));
    for(unsigned j=0;j<2;++j) {
        printf("H%u:",j);
        for(unsigned i=0;i<8;++i)printf(" %02X",boot51[j][i]);
        printf("\n");
    }
    unsigned first;
    printf("MATCH I/A/B %u/%u/%u OF512\n",BYTES51-diff51(boot51[0],INITIAL51,&first),
           BYTES51-diff51(boot51[0],A51,&first),BYTES51-diff51(boot51[0],B51,&first));
    printf("WRITE CALLS %u  FLUSH %u\n",write_calls,flush_calls);
    printf("BIND %u READ %u KEY %04lX\n",bind_calls,read_calls,(unsigned long)gate.keys);
    printf("A/B EDGE %04lX/%04lX LAST %04lX\n",(unsigned long)(gate.a_edges&65535u),
           (unsigned long)(gate.b_edges&65535u),(unsigned long)gate.last_nonzero);
    if(!eligible)printf("WRITE LOCK: DISABLED\nREAD/CONTENT CHECK FAILED\nSEND THIS SCREEN; NO RETRY\n");
    else switch(gate.state) {
    case G_QUIET:printf("WRITE LOCK: LOCKED\nRELEASE ALL BUTTONS\nWAIT FOR QUIET 0.5 SECOND\n");break;
    case G_LOCKED:printf("WRITE LOCK: LOCKED\nFIRST TEST: SEND THIS SCREEN\nL+R HOLD 2 SEC TO UNLOCK\n");break;
    case G_HOLD_LR:printf("WRITE LOCK: LOCKED\nKEEP HOLDING L+R FOR 2 SEC\nA/B ALONE CANNOT WRITE\n");break;
    case G_RELEASE_LR:printf("UNLOCKED - NOT CONFIRMED\nRELEASE ALL FOR 0.5 SECOND\nNO WRITE YET\n");break;
    case G_CHOOSE:printf("SELECT MODE (10 SEC WINDOW)\nA: WRITE A WITHOUT FLUSH\nB: WRITE B THEN FLUSH\n");break;
    case G_RELEASE_MODE:printf("SELECTED %c - NOT CONFIRMED\nRELEASE ALL FOR 0.5 SECOND\nNO WRITE YET\n",gate.mode==A51?'A':'B');break;
    case G_CONFIRM:printf("CONFIRM MODE %c\nHOLD START (+) 1 SECOND\nOTHER KEY CANCELS; 10 SEC LIMIT\n",gate.mode==A51?'A':'B');break;
    case G_HOLD_START:printf("CONFIRMING MODE %c\nKEEP START (+) HELD 1 SECOND\nRELEASE EARLY CANCELS\n",gate.mode==A51?'A':'B');break;
    case G_DONE:printf("ONE-SHOT PERMISSION CONSUMED\nNO SECOND WRITE THIS BOOT\n\n");break;
    }
    printf("BOOT CONTENT = PRE-WRITE READ\n");fflush(stdout);
}
static void progress51(enum step51 s) {
    title51();
    const char *label=s==S51_BIND?"BIND SLOT4 (NO HOST OPEN)":s==S51_WRITE?"WRITE512...":
        s==S51_READ?"READ512 + BYTE COMPARE...":s==S51_FLUSH?"FLUSH... UP TO 60 SECONDS":"SEQUENCE COMPLETE";
    printf("CONFIRMED MODE %c\n%s\n",gate.mode==A51?'A':'B',label);fflush(stdout);
}
static void result_page51(const struct result51 *r,unsigned page) {
    title51();printf("MODE %c  PAGE %u/2\n",r->mode==A51?'A':'B',page+1);
    if(!page) {
        printf("BOOT CONTENT: %s\n",name51(boot_kind));
        printf("UNLOCK %u CONFIRM %u USED %u\n",gate.unlocks,gate.confirmations,gate.taken);
        printf("BIND %u WRITE %u READ %u FLUSH %u\n",bind_calls,write_calls,read_calls,flush_calls);
        printf("BIND RC %d (NO HOST OPEN)\n",r->bind_rc);
        if(r->attempted&2)printf("WRITE512 RC %d\n",r->write_rc);else printf("WRITE NOT ISSUED\n");
        if(r->attempted&4)printf("READ512 RC %d\n",r->read_rc);else printf("READ NOT ISSUED\n");
        if((r->attempted&4) && !r->read_rc) {
            printf("MATCH %u/512 FIRST BAD %u\n",BYTES51-r->mismatches,r->first_bad);
            printf("READBACK CRC %08lX\n",(unsigned long)crc51(read51,BYTES51));
        } else printf("READBACK NOT VERIFIED\n");
        if(r->attempted&8)printf("FLUSH RC %d\n",r->flush_rc);else printf("FLUSH NOT ISSUED\n");
        printf("SEQUENCE %s RC %d\n",r->rc?"FAIL":"PASS",r->rc);
        printf("PERSISTENCE: NOT YET TESTED\nQUIT; POWER OFF/ON; OPEN 51\nDO NOT RECOPY static51.bin\n");
    } else {
        printf("FLUSH OBSERVATION SNAP RC %d\n",snap.rc);
        if(!(r->attempted&8))printf("FLUSH WAS NOT ISSUED\n");
        else if(!forensic_header_valid(&snap))printf("SNAP HEADER NOT VALIDATED\n");
        else {
            const uint32_t *s=snap.raw+40;
            printf("F8 RD %lu TX %lu\n",(unsigned long)s[0],(unsigned long)s[1]);
            printf("HOST WR %lu TABLE RD %lu\n",(unsigned long)s[2],(unsigned long)s[3]);
            printf("BUSY %lu DONE %lu\n",(unsigned long)s[7],(unsigned long)s[8]);
            printf("CMD %08lX -> %08lX\n",(unsigned long)s[24],(unsigned long)s[12]);
            printf("STATUS %08lX\n",(unsigned long)s[9]);
            printf("SLOT %lu SIZE SEEN %lu\n",(unsigned long)s[13],(unsigned long)((s[15]&4)!=0));
            printf("TX ABORT %lu LOST %lu\n",(unsigned long)s[17],(unsigned long)s[30]);
            printf("TX != HOST DECODE CONFIRMED\n");
        }
    }
    printf("LEFT/RIGHT PAGE; A SUMMARY\n");fflush(stdout);
}
#ifndef OF_PC
#include "of_file.h"
#include "of_input.h"
static uint32_t now51(void){return OF_SVC->timer_get_ms();}
static void idle51(void){usleep(1000);}
static int sdk_read51(uint8_t *out) {
    const struct read50_ops o={of_file_read_async,of_file_async_poll,of_file_async_busy,now51,idle51};
    ++read_calls;return read512_run(&o,out);
}
static int sdk_bind51(void) {
    ++bind_calls;
    /* The existing static50 service's name is an authorization token, NOT a
     * filename used by the host. It binds the selected 512-byte slot 4 only.
     * Reuse its verified OS bytes; the actual instance file is static51.bin. */
    return of_static50_bind();
}
static int sdk_write51(uint32_t off,const void *src,uint32_t len) {
    ++write_calls;return of_output_write(off,src,len);
}
static int sdk_flush51(void){++flush_calls;return of_output_flush();}
static void stop51(const char *stage,int rc) {
    printf("STOP %s RC %d\nNO WRITE ISSUED\n",stage,rc);fflush(stdout);
    while(1)usleep(100000);
}
#endif
int main(int argc,char **argv) {
#ifdef OF_PC
    (void)run51;(void)read512_run;(void)update51;
    if(argc>1 && !strcmp(argv[1],"--demo")) {
        boot_rc51[0]=boot_rc51[1]=0;
        fill51(boot51[0],INITIAL51);fill51(boot51[1],INITIAL51);
        classify_boot51();read_calls=2;
        for(unsigned state=G_QUIET;state<=G_DONE;++state) {
            gate.state=state;gate.mode=B51;show_guard51();
        }
        for(unsigned kind=0;kind<4;++kind) {
            unsigned tag=kind==0?A51:kind==1?B51:INITIAL51;
            fill51(boot51[0],tag);fill51(boot51[1],tag);
            if(kind==2)boot51[1][511]^=1;
            if(kind==3)boot_rc51[1]=-12;
            classify_boot51();gate.state=G_LOCKED;show_guard51();
        }
        for(unsigned c=0;c<7;++c) {
            struct result51 r={0};r.mode=c?B51:A51;r.attempted=c?15:7;
            r.first_bad=512;r.flush_rc=c==1?-12:0;r.rc=r.flush_rc;
            if(c==2){r.attempted=7;r.mismatches=512;r.first_bad=0;r.rc=-1;}
            if(c==3){r.attempted=3;r.write_rc=-2;r.rc=-2;}
            if(c==4){r.attempted=7;r.read_rc=-12;r.rc=-12;}
            if(c==5){r.attempted=1;r.bind_rc=-11;r.rc=-11;}
            gate.unlocks=9999;gate.confirmations=gate.taken=1;
            memset(&snap,0,sizeof(snap));snap.words_read=72;snap.header_checked=1;
            for(unsigned i=40;i<72;++i)snap.raw[i]=0xffffffffu;
            result_page51(&r,0);result_page51(&r,1);
        }
        for(unsigned s=S51_BIND;s<=S51_DONE;++s)progress51(s);
        return 0;
    }
    int pass=of_static50_query()==OF_ERR_NOT_SUPPORTED && of_static50_bind()==OF_ERR_NOT_SUPPORTED;
    for(unsigned mode=0;mode<3;++mode) {
        unsigned tag=mode==0?INITIAL51:mode==1?A51:B51;
        boot_rc51[0]=boot_rc51[1]=0;fill51(boot51[0],tag);fill51(boot51[1],tag);
        classify_boot51();pass=pass && eligible && repeat_ok && boot_kind==tag;
        for(unsigned i=0;i<BYTES51;++i) {
            boot51[1][i]^=1;classify_boot51();pass=pass && !eligible && !repeat_ok;
            boot51[1][i]^=1;
        }
        for(unsigned i=0;i<2;++i) {
            boot_rc51[i]=-12;classify_boot51();pass=pass && !eligible && !boot_kind;
            boot_rc51[i]=0;
        }
    }
    memset(boot51,0,sizeof(boot51));classify_boot51();pass=pass && !eligible;
    printf("Guard51 PC service/boot double-read gate: %s\n",pass?"PASS":"FAIL");return !pass;
#else
    (void)argc;(void)argv;title51();fflush(stdout);
    if(!of_output_available() || !OF_OUTPUT_SVC_HAS(file_size) ||
       !OF_OUTPUT_SVC_HAS(timer_get_ms) || !OF_OUTPUT_SVC_HAS(input_poll_p0))stop51("API",-8);
    int query=of_static50_query();
    if(query!=OF_STATIC50_MAGIC)stop51("WRONG OS",query);
    long size=OF_SVC->file_size("slot:4");
    if(size!=BYTES51)stop51("SLOT4 SIZE",(int)size);
    printf("BOOT READ512 TWICE; NO WRITE...\n");fflush(stdout);
    boot_rc51[0]=sdk_read51(boot51[0]);
    if(!boot_rc51[0])boot_rc51[1]=sdk_read51(boot51[1]);
    classify_boot51();show_guard51();
    if(!eligible)while(1)usleep(100000);
    uint32_t shown_keys=0,shown_edges=0,shown_time=now51();
    while(1) {
        of_input_poll_p0();enum gate51 prev=gate.state;uint32_t now=now51();
        if(update51(&gate,_of_input_p0.buttons,now))break;
        if(prev!=gate.state || ((uint32_t)(now-shown_time)>=150u &&
           (shown_keys!=gate.keys || shown_edges!=gate.a_edges+gate.b_edges))) {
            show_guard51();shown_keys=gate.keys;shown_edges=gate.a_edges+gate.b_edges;shown_time=now;
        }
        usleep(16000);
    }
    int rc=of_bridge_audit_begin_with(of_bridge_trace_read);
    if(rc)stop51("AUDIT ARM",rc);
    const struct ops51 o={sdk_bind51,sdk_write51,sdk_read51,sdk_flush51,progress51};
    struct result51 r=run51(&gate,&o);
    forensic_snapshot_read(&snap,of_bridge_trace_read);
    unsigned page=0;result_page51(&r,page);
    uint32_t last=gate.keys;
    while(1) {
        of_input_poll_p0();uint32_t keys=_of_input_p0.buttons&65535u;
        uint32_t edges=keys&~last;last=keys;unsigned previous=page;
        if(edges&(OF_BTN_LEFT|OF_BTN_RIGHT))page^=1;
        if(edges&OF_BTN_A)page=0;
        if(page!=previous)result_page51(&r,page);
        usleep(16000);
    }
#endif
}
