#include <stdio.h>
#include <unistd.h>
#include "of_output.h"
#include "probe.h"
#include "../storage_slot_probe/probe.h"
#ifndef OF_PC
#include "of_input.h"
static struct audit_snapshot snapshot;
static int api_available(void){return of_output_available() && OF_OUTPUT_SVC_HAS(file_size);}
static long slot_size(const char *p){return OF_SVC->file_size(p);}
static void pre_open(const struct slot_probe_result *r){
    printf("OS SLOT1 %ld\nSLOT4 PRE %ld\nOPEN...\n",r->os_size,r->slot_pre);fflush(stdout);
}
static void pre_flush(const struct slot_probe_result *r){
    printf("OPEN %d  POST %ld\nFLUSH NO WRITE...\n",r->open_rc,r->slot_post);fflush(stdout);
}
static const char *kind_name(unsigned k){
    return k==1?"RD":k==2?"TX":k==3?"WR":k==4?"?T":k==5?"AB":k==14?"PH":"AR";
}
static void show_page(unsigned page,const struct slot_probe_result *r){
    unsigned pages=3+(snapshot.count+5)/6;
    const uint32_t *o=snapshot.header+8,*f=snapshot.header+40;
    printf("\033[2J\033[H TF AUDIT smoke.47 %u/%u\n",page+1,pages);
    if(page==0){
        printf("KEEP CARTRIDGE EMPTY\nOPEN %d  FLUSH %d\n",r->open_rc,r->flush_rc);
        printf("SNAP %d N %lu\nFLAGS %08lX\nST %08lX\n",snapshot.rc,(unsigned long)snapshot.count,(unsigned long)snapshot.flags,(unsigned long)snapshot.legacy);
        if(!snapshot.rc){
            printf("CM/BU/OK  (COUNTS)\nOPEN  %lu/%lu/%lu\nFLUSH %lu/%lu/%lu\n",(unsigned long)o[11],(unsigned long)o[7],(unsigned long)o[8],(unsigned long)f[11],(unsigned long)f[7],(unsigned long)f[8]);
            printf("TABLE RD %lu/%lu\nTX ABORT %lu/%lu\n",(unsigned long)o[3],(unsigned long)f[3],(unsigned long)o[17],(unsigned long)f[17]);
            printf("CAPTURE %lu MS\n",(unsigned long)(snapshot.header[4]/74250u));
            printf("TX != HOST CONFIRM\n");
        }
    }else if(page<=2){
        const uint32_t *s=page==1?o:f;
        printf("%s WINDOW\n",page==1?"OPEN":"FLUSH");
        if(snapshot.rc){printf("SNAPSHOT UNAVAILABLE RC %d\n",snapshot.rc);}
        else{
            printf("F8 R/T/W %lu/%lu/%lu\n",(unsigned long)s[0],(unsigned long)s[1],(unsigned long)s[2]);
            printf("CM/BU/OK %lu/%lu/%lu\n",(unsigned long)s[11],(unsigned long)s[7],(unsigned long)s[8]);
            printf("CMD %08lX -> %08lX\n",(unsigned long)s[24],(unsigned long)s[12]);
            printf("STATUS WR %lu LAST %08lX\n",(unsigned long)s[6],(unsigned long)s[9]);
            printf("SLOT %s %08lX\n",s[15]&1?"YES":"NO",(unsigned long)s[13]);
            printf("SIZE %s %lu\n",s[15]&4?"YES":"NO",(unsigned long)s[14]);
            printf("TABLE RD %lu\n",(unsigned long)s[3]);
            printf("ALL TX Q/D/A %lu/%lu/%lu\n",(unsigned long)s[26],(unsigned long)s[16],(unsigned long)s[17]);
            printf("LAST RD %08lX\n",(unsigned long)s[22]);
            printf("LAST TX %08lX %08lX\n",(unsigned long)s[23],(unsigned long)s[28]);
            printf("OTHER R/T %lu/%lu LOST %lu\n",(unsigned long)s[20],(unsigned long)s[21],(unsigned long)s[30]);
            printf("TICKS %08lX-%08lX\n",(unsigned long)s[18],(unsigned long)s[19]);
        }
    }else{
        unsigned first=(page-3)*6;
        printf("# PH TYPE T64 / ADDR DATA NEXT\n");
        for(unsigned i=first;i<snapshot.count && i<first+6;++i){
            const uint32_t *e=snapshot.event[i].word;
            unsigned p=(e[0]>>26)&3;
            printf("%03u %c %s %07lX\n%08lX %08lX %08lX\n",i,p==1?'O':p==2?'F':'-',kind_name(e[0]>>28),
                (unsigned long)(e[0]&0x3ffffffu),(unsigned long)e[1],(unsigned long)e[2],(unsigned long)e[3]);
        }
    }
    printf("\nLEFT/RIGHT PAGE  A SUMMARY\n");fflush(stdout);
}
#endif
int main(void){
#ifdef OF_PC
    int pass=of_bridge_audit_begin_with(of_bridge_trace_read)==OF_ERR_NOT_SUPPORTED;
    printf("Audit PC unsupported fallback: %s\n",pass?"PASS":"FAIL");return !pass;
#else
    printf("\033[2J\033[H TF AUDIT smoke.47\nKEEP CARTRIDGE EMPTY\n");fflush(stdout);
    int rc=of_bridge_audit_begin_with(of_bridge_trace_read);
    if(rc){
        printf("AUDIT BUILD/ARM FAIL RC %d\nNO FILE COMMANDS ISSUED\n",rc);fflush(stdout);
        while(1)usleep(100000);
    }
    const struct slot_probe_api api={api_available,slot_size,of_output_open,of_output_flush};
    struct slot_probe_result r=slot_probe_run(&api,pre_open,pre_flush);
    audit_snapshot_read(&snapshot,of_bridge_trace_read);
    unsigned page=0,pages=3+(snapshot.count+5)/6;show_page(page,&r);
    while(1){
        of_input_poll_p0();unsigned old=page;
        if(of_btn_pressed(OF_BTN_RIGHT))page=(page+1)%pages;
        if(of_btn_pressed(OF_BTN_LEFT))page=(page+pages-1)%pages;
        if(of_btn_pressed(OF_BTN_A))page=0;
        if(page!=old)show_page(page,&r);
        usleep(16000);
    }
#endif
}
