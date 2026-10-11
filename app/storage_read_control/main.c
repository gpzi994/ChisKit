#include <stdio.h>
#include <unistd.h>
#include "of_output.h"
#include "../storage_audit_probe/probe.h"
#ifndef OF_PC
#include "of_file.h"
#include "of_input.h"
#include "probe.h"
static struct audit_snapshot snapshot;
static int api_available(void) {
    return of_output_available() && OF_OUTPUT_SVC_HAS(file_size) &&
           OF_OUTPUT_SVC_HAS(timer_get_ms);
}
static long slot_size(const char *p){return OF_SVC->file_size(p);}
static uint32_t now_ms(void){return OF_SVC->timer_get_ms();}
static void idle(void){usleep(1000);}
static struct read4_result sdk_read4(void) {
    const struct read4_ops ops={of_file_read_async,of_file_async_poll,
                                of_file_async_busy,now_ms,idle};
    return read4_run(&ops);
}
static void progress(enum read_control_stage stage,const struct read_control_result *r) {
    if (stage==RC_OPEN)
        printf("OS %ld PRE %ld\nOPEN...\n",r->os_size,r->slot_pre);
    if (stage==RC_READ4)
        printf("OPEN %d POST %ld\nREAD4 SLOT4 OFFSET0...\n",r->open_rc,r->slot_post);
    if (stage==RC_FLUSH)
        printf("READ4 RC %d\nFLUSH NO WRITE...\n",r->read.rc);
    fflush(stdout);
}
static const char *kind_name(unsigned k) {
    return k==1?"RD":k==2?"TX":k==3?"WR":k==4?"?T":k==5?"AB":k==14?"PH":"AR";
}
static int read_cmd_seen(void) {
    if (snapshot.rc) return 0;
    for (unsigned i=0;i<snapshot.count;++i) {
        const uint32_t *w=snapshot.event[i].word;
        if ((w[0]>>28)==2 && ((w[0]>>26)&3)==1 &&
            (w[1]&0xff00ffffu)==0xf8001000u &&
            (w[2]==0x636d0180u || w[2]==0x636d0181u)) return 1;
    }
    return 0; /* NO = absent from saved events, not proof of no command. */
}
static void show_page(unsigned page,const struct read_control_result *r) {
    unsigned pages=3+(snapshot.count+5)/6;
    const uint32_t *o=snapshot.header+8,*f=snapshot.header+40;
    printf("\033[2J\033[H TF READ4 smoke.48 %u/%u\n",page+1,pages);
    if (page==0) {
        printf("KEEP CARTRIDGE EMPTY\n");
        if(r->open_attempted) printf("OPEN RC %d POST %ld\n",r->open_rc,r->slot_post);
        else printf("OPEN SKIP API%d OS%ld\n",r->api_ok,r->os_size);
        if(r->read_attempted) {
            printf("READ4 %s RC %d\n",r->read.rc==0?"PASS":"FAIL",r->read.rc);
            printf("START %s TOKEN %d\n",r->read.start_attempted?"YES":"NO",r->read.token);
            printf("CB %u TOKEN %d RC %d\n",r->read.callbacks,r->read.callback_token,r->read.callback_rc);
            printf("READ MS %lu\n",(unsigned long)r->read.elapsed_ms);
            if(!r->read.rc) printf("BYTES %02X %02X %02X %02X\n",r->read.bytes[0],r->read.bytes[1],r->read.bytes[2],r->read.bytes[3]);
        } else printf("READ4 SKIP\n");
        if(r->flush_attempted) printf("FLUSH RC %d\n",r->flush_rc);
        else printf("FLUSH SKIP - TEST STOPPED\n");
        printf("SNAP %d N%lu FLAGS %08lX\n",snapshot.rc,(unsigned long)snapshot.count,(unsigned long)snapshot.flags);
        if(!snapshot.rc) {
            printf("ST %08lX RD CMD SEEN %s\n",(unsigned long)snapshot.legacy,read_cmd_seen()?"YES":"NO");
            printf("OR BU/OK %lu/%lu\nFL BU/OK %lu/%lu\n",(unsigned long)o[7],(unsigned long)o[8],(unsigned long)f[7],(unsigned long)f[8]);
        }
        printf("READ CONTENT NOT VERIFIED\n");
    } else if(page<=2) {
        const uint32_t *s=page==1?o:f;
        printf("%s WINDOW\n",page==1?"OPEN+READ":"FLUSH");
        if(page==2 && !r->flush_attempted) printf("FLUSH WAS NOT ISSUED\n");
        if(snapshot.rc) printf("SNAPSHOT UNAVAILABLE RC %d\n",snapshot.rc);
        else {
            printf("F8 R/T/W %lu/%lu/%lu\n",(unsigned long)s[0],(unsigned long)s[1],(unsigned long)s[2]);
            printf("%s-CM/BU/OK %lu/%lu/%lu\n",page==1?"O":"F",(unsigned long)s[11],(unsigned long)s[7],(unsigned long)s[8]);
            printf("CMD %08lX -> %08lX\n",(unsigned long)s[24],(unsigned long)s[12]);
            printf("STATUS WR %lu LAST %08lX\n",(unsigned long)s[6],(unsigned long)s[9]);
            printf("SLOT %s %08lX\n",s[15]&1?"YES":"NO",(unsigned long)s[13]);
            printf("SIZE %s %lu TABLE RD %lu\n",s[15]&4?"YES":"NO",(unsigned long)s[14],(unsigned long)s[3]);
            printf("ALL TX Q/D/A %lu/%lu/%lu\n",(unsigned long)s[26],(unsigned long)s[16],(unsigned long)s[17]);
            printf("LAST RD %08lX\nLAST TX %08lX %08lX\n",(unsigned long)s[22],(unsigned long)s[23],(unsigned long)s[28]);
            printf("OTHER R/T %lu/%lu LOST %lu\n",(unsigned long)s[20],(unsigned long)s[21],(unsigned long)s[30]);
            printf("TICKS %08lX-%08lX\n",(unsigned long)s[18],(unsigned long)s[19]);
            if(page==1) printf("BU/OK INCLUDE OPEN AND READ\n");
        }
    } else {
        unsigned first=(page-3)*6;
        printf("# PH TYPE T64 / ADDR DATA NEXT\n");
        for(unsigned i=first;i<snapshot.count && i<first+6;++i) {
            const uint32_t *e=snapshot.event[i].word; unsigned p=(e[0]>>26)&3;
            printf("%03u %c %s %07lX\n%08lX %08lX %08lX\n",i,p==1?'O':p==2?'F':'-',kind_name(e[0]>>28),
                (unsigned long)(e[0]&0x3ffffffu),(unsigned long)e[1],(unsigned long)e[2],(unsigned long)e[3]);
        }
    }
    printf("\nLEFT/RIGHT PAGE  A SUMMARY\n");fflush(stdout);
}
#endif
int main(void) {
#ifdef OF_PC
    int pass=of_bridge_audit_begin_with(of_bridge_trace_read)==OF_ERR_NOT_SUPPORTED;
    printf("Read4 PC unsupported fallback: %s\n",pass?"PASS":"FAIL");return !pass;
#else
    printf("\033[2J\033[H TF READ4 smoke.48\nKEEP CARTRIDGE EMPTY\n");fflush(stdout);
    int rc=of_bridge_audit_begin_with(of_bridge_trace_read);
    if(rc) {
        printf("AUDIT BUILD/ARM FAIL RC %d\nNO FILE COMMANDS ISSUED\n",rc);fflush(stdout);
        while(1)usleep(100000);
    }
    const struct read_control_api api={api_available,slot_size,of_output_open,sdk_read4,of_output_flush};
    struct read_control_result r=read_control_run(&api,progress);
    audit_snapshot_read(&snapshot,of_bridge_trace_read);
    unsigned page=0,pages=3+(snapshot.count+5)/6;show_page(page,&r);
    while(1) {
        of_input_poll_p0();unsigned old=page;
        if(of_btn_pressed(OF_BTN_RIGHT))page=(page+1)%pages;
        if(of_btn_pressed(OF_BTN_LEFT))page=(page+pages-1)%pages;
        if(of_btn_pressed(OF_BTN_A))page=0;
        if(page!=old)show_page(page,&r);
        usleep(16000);
    }
#endif
}
