#include <stdio.h>
#include <unistd.h>
#include "of_output.h"
#include "snapshot.h"
#include "../storage_read_control/probe.h"
#ifndef OF_PC
#include "of_file.h"
#include "of_input.h"
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
    if(stage==RC_OPEN) printf("OS %ld PRE %ld\nOPEN...\n",r->os_size,r->slot_pre);
    if(stage==RC_READ4) printf("OPEN %d POST %ld\nREAD4 SLOT4 OFFSET0...\n",r->open_rc,r->slot_post);
    if(stage==RC_FLUSH) printf("READ4 RC %d\nFLUSH NO WRITE...\n",r->read.rc);
    fflush(stdout);
}
#endif
static struct forensic_snapshot snapshot;
static unsigned page_count(void){return 5+(snapshot.words_read+23)/24;}
static const char *header_state(void) {
    return !snapshot.header_checked?"INCOMPLETE":snapshot.bad_mask?"BAD":"OK";
}
static void raw_lines(unsigned first,unsigned end) {
    if(end>snapshot.words_read) end=snapshot.words_read;
    for(unsigned i=first;i<end;i+=3) {
        printf("%04u",i);
        for(unsigned j=i;j<end && j<i+3;++j) printf(" %08lX",(unsigned long)snapshot.raw[j]);
        printf("\n");
    }
}
static void show_header(void) {
    const uint32_t expected[8]={OF_BRIDGE_AUDIT_STREAM_MAGIC,1,72,
        snapshot.flags&0xffffu,0,snapshot.flags&0xffff0000u,2,0};
    printf("H# ACTUAL   EXPECTED / RULE\n");
    for(unsigned i=0;i<8;++i) {
        if(i>=snapshot.words_read) {printf("%02u -------- UNREAD\n",i);continue;}
        printf("%02u %08lX ",i,(unsigned long)snapshot.raw[i]);
        if(i==4) printf("TICKS (ANY)");
        else if(i==7) printf("RESERVED (RAW)");
        else if(!snapshot.flags_read && (i==3 || i==5)) printf("COUNT UNKNOWN");
        else if(i==6) printf("<=00000002");
        else printf("%08lX",(unsigned long)expected[i]);
        if(snapshot.header_checked && i!=4 && i!=7)
            printf(" %s",snapshot.bad_mask&(1u<<i)?"BAD":"OK");
        printf("\n");
    }
    printf("HEADER %s MASK %02X\n",header_state(),snapshot.bad_mask);
    printf("BITS: 0 MAGIC 1 VER 2 LENGTH\n3 COUNT 5 FLAGS 6 PHASE\n");
    printf("NO REWIND OR DATA RETRY\n");
}
static void show_details(void) {
    printf("STAGE %s RC %d\n",forensic_stage_name(snapshot.stage),snapshot.rc);
    printf("API %d REG %u CALL %u\n",snapshot.api_rc,snapshot.selector,snapshot.calls);
    if(snapshot.failure_word==FORENSIC_NO_WORD) printf("FAILED DATA WORD: NONE\n");
    else printf("FAILED DATA WORD: %lu\n",(unsigned long)snapshot.failure_word);
    if(snapshot.first_bad==FORENSIC_NO_WORD) printf("FIRST BAD HEADER: NONE\n");
    else printf("FIRST BAD HEADER: %lu\n",(unsigned long)snapshot.first_bad);
    printf("FREEZE %08lX POLLS %u\n",(unsigned long)snapshot.ready,snapshot.freeze_polls);
    if(snapshot.legacy_read) printf("LEGACY %08lX\n",(unsigned long)snapshot.legacy);
    else printf("LEGACY UNREAD\n");
    unsigned h=snapshot.words_read<72?snapshot.words_read:72;
    unsigned tail=snapshot.words_read>72?(snapshot.words_read-72)%4:0;
    printf("HEADER %u/72 %s\n",h,header_state());
    printf("RAW GROUPS %u TAIL %u WORDS\n",forensic_raw_groups(&snapshot),tail);
    printf("LAST <=6 SUCCESSFUL RAW WORDS\n");
    raw_lines(snapshot.words_read>6?snapshot.words_read-6:0,snapshot.words_read);
    if(!snapshot.words_read) printf("NONE - NO STREAM WORD READ\n");
    printf("RAW GROUP != VALIDATED EVENT\n");
}
static void show_window(unsigned which,const struct read_control_result *r) {
    printf("%s WINDOW\n",which==0?"OPEN+READ":"FLUSH");
    if(which==1 && !r->flush_attempted) printf("FLUSH WAS NOT ISSUED\n");
    if(!forensic_header_valid(&snapshot)) {
        printf("HEADER NOT VALIDATED\nNO COUNTER INTERPRETATION\nRAW DATA ON PAGES 6+\n");
        return;
    }
    const uint32_t *s=snapshot.raw+(which==0?8:40);
    printf("F8 RD %lu TX %lu\n",(unsigned long)s[0],(unsigned long)s[1]);
    printf("F8 WR %lu TABLE %lu\n",(unsigned long)s[2],(unsigned long)s[3]);
    printf("BU %lu OK %lu\n",(unsigned long)s[7],(unsigned long)s[8]);
    printf("CMD %08lX -> %08lX\n",(unsigned long)s[24],(unsigned long)s[12]);
    printf("STATUS WR %lu LAST %08lX\n",(unsigned long)s[6],(unsigned long)s[9]);
    printf("SLOT %s %08lX\n",s[15]&1?"YES":"NO",(unsigned long)s[13]);
    printf("SIZE %s %lu\n",s[15]&4?"YES":"NO",(unsigned long)s[14]);
    printf("TX Q %lu D %lu\n",(unsigned long)s[26],(unsigned long)s[16]);
    printf("TX ABORT %lu LOST %lu\n",(unsigned long)s[17],(unsigned long)s[30]);
    printf("LAST RD %08lX\n",(unsigned long)s[22]);
    printf("LAST TX %08lX %08lX\n",(unsigned long)s[23],(unsigned long)s[28]);
    if(which==0) printf("BU/OK INCLUDE OPEN AND READ\n");
    printf("TX != HOST DECODE CONFIRMED\n");
}
static void show_page(unsigned page,const struct read_control_result *r) {
    printf("\033[2J\033[H TF SNAP smoke.49 %u/%u\n",page+1,page_count());
    if(page==0) {
        printf("KEEP CARTRIDGE EMPTY\n");
        if(r->open_attempted) printf("OPEN RC %d POST %ld\n",r->open_rc,r->slot_post);
        else printf("OPEN SKIP API%d OS%ld\n",r->api_ok,r->os_size);
        if(r->read_attempted) {
            printf("READ4 RC %d START %d TOKEN %d\n",r->read.rc,r->read.start_attempted,r->read.token);
            printf("CB %u TOKEN %d RC %d\n",r->read.callbacks,r->read.callback_token,r->read.callback_rc);
            printf("READ MS %lu\n",(unsigned long)r->read.elapsed_ms);
        } else printf("READ4 SKIP\n");
        if(r->flush_attempted) printf("FLUSH RC %d\n",r->flush_rc);
        else printf("FLUSH SKIP - TEST STOPPED\n");
        printf("SNAP RC %d API %d\n",snapshot.rc,snapshot.api_rc);
        printf("STAGE %s\n",forensic_stage_name(snapshot.stage));
        if(snapshot.flags_read) printf("FLAGS %08lX\n",(unsigned long)snapshot.flags);
        else printf("FLAGS UNREAD\n");
        if(snapshot.count_valid) printf("CAP %lu RAW GROUPS %u\n",(unsigned long)(snapshot.flags&0xffffu),forensic_raw_groups(&snapshot));
        else printf("CAP UNKNOWN / INVALID\n");
        printf("WORDS %u/%u HDR %s\n",snapshot.words_read,snapshot.expected_words,header_state());
        printf("%s\n",snapshot.rc?"SNAPSHOT NOT VALIDATED":"SNAPSHOT VALIDATED");
        printf("READ CONTENT NOT VERIFIED\n");
    } else if(page==1) show_header();
    else if(page==2) show_details();
    else if(page<=4) show_window(page-3,r);
    else {
        unsigned first=(page-5)*24;
        printf("RAW STREAM - NO RESYNC\nWORD INDEX / 3 VALUES PER ROW\n");
        raw_lines(first,first+24);
        printf("0000-0071 HEADER (72 WORDS)\n0072+ EVENT AREA (4 WORDS EACH)\n");
        printf("%s\n",snapshot.rc?"UNVALIDATED RAW DATA":"COMPLETE VALIDATED SNAPSHOT");
    }
    printf("\nLEFT/RIGHT PAGE  A SUMMARY\n");fflush(stdout);
}
#ifdef OF_PC
static void demo(unsigned mode) {
    struct read_control_result r={0};
    r.open_attempted=r.read_attempted=r.read.start_attempted=1;
    r.slot_post=1048613;r.read.rc=r.read.callback_rc=-2;
    r.read.callbacks=1;r.read.elapsed_ms=18;
    memset(&snapshot,0,sizeof(snapshot));
    snapshot.failure_word=snapshot.first_bad=FORENSIC_NO_WORD;
    snapshot.flags_read=snapshot.count_valid=snapshot.legacy_read=1;
    snapshot.flags=0x40025;snapshot.legacy=0xdf3f1c60;
    snapshot.expected_words=220;snapshot.words_read=220;
    snapshot.freeze_polls=snapshot.ready=1;
    snapshot.selector=OF_BRIDGE_TRACE_DATA;snapshot.calls=223;
    for(unsigned i=0;i<220;++i)snapshot.raw[i]=0x80000000u+i;
    snapshot.raw[0]=OF_BRIDGE_AUDIT_STREAM_MAGIC;snapshot.raw[1]=1;
    snapshot.raw[2]=72;snapshot.raw[3]=38; /* Deliberate synthetic H3 mismatch. */
    snapshot.raw[5]=1u<<18;snapshot.raw[6]=1;snapshot.raw[7]=0;
    snapshot.header_checked=1;snapshot.bad_mask=1u<<3;snapshot.first_bad=3;
    snapshot.rc=-1;snapshot.stage=FS_HEADER_CHECK;
    if(mode==1) {
        snapshot.words_read=2;snapshot.failure_word=2;snapshot.calls=6;
        snapshot.header_checked=snapshot.bad_mask=0;snapshot.first_bad=FORENSIC_NO_WORD;
        snapshot.rc=snapshot.api_rc=-17;snapshot.stage=FS_HEADER;
    } else if(mode==2) {
        snapshot.flags=0x70000|256;snapshot.expected_words=snapshot.words_read=1096;
        for(unsigned i=8;i<1096;++i)snapshot.raw[i]=0xffffffffu;
        snapshot.raw[3]=256;snapshot.raw[5]=0x70000;
        snapshot.bad_mask=0;snapshot.first_bad=FORENSIC_NO_WORD;
        snapshot.rc=0;snapshot.stage=FS_DONE;
        r.read.rc=r.read.callback_rc=0;r.flush_attempted=1;r.flush_rc=-12;
    }
    for(unsigned page=0;page<page_count();++page)show_page(page,&r);
}
#endif
int main(int argc,char **argv) {
#ifdef OF_PC
    (void)read_control_run;(void)read4_run;
    if(argc>1 && !strcmp(argv[1],"--demo")) {
        for(unsigned mode=0;mode<3;++mode)demo(mode);
        return 0;
    }
    int pass=of_bridge_audit_begin_with(of_bridge_trace_read)==OF_ERR_NOT_SUPPORTED;
    printf("Snapshot forensics PC unsupported: %s\n",pass?"PASS":"FAIL");return !pass;
#else
    (void)argc;(void)argv;
    printf("\033[2J\033[H TF SNAP smoke.49\nKEEP CARTRIDGE EMPTY\n");fflush(stdout);
    int rc=of_bridge_audit_begin_with(of_bridge_trace_read);
    if(rc) {
        printf("AUDIT BUILD/ARM FAIL RC %d\nNO FILE COMMANDS ISSUED\n",rc);fflush(stdout);
        while(1)usleep(100000);
    }
    const struct read_control_api api={api_available,slot_size,of_output_open,sdk_read4,of_output_flush};
    struct read_control_result r=read_control_run(&api,progress);
    forensic_snapshot_read(&snapshot,of_bridge_trace_read);
    unsigned page=0,pages=page_count();show_page(page,&r);
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
