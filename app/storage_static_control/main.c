#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "of_static_output_test.h"
#include "sequence.h"
#include "read512.h"
#include "../storage_snapshot_forensics/snapshot.h"
static struct forensic_snapshot snap;
static uint8_t boot_bytes[512] __attribute__((aligned(64)));
static unsigned boot_pattern;
static int boot_rc;
static const char *pattern_name(unsigned p) {
    return p==P_SEED?"INITIAL":p==P_A?"A VERIFIED":p==P_B?"B VERIFIED":"UNKNOWN";
}
static void title(void) {printf("\033[2J\033[H TF STATIC smoke.50\nKEEP CARTRIDGE EMPTY\n");}
static void show_menu(void) {
    title();printf("OS STATIC50 OK  SLOT4 512\n");
    printf("BOOT READ RC %d\nBOOT CONTENT: %s\n",boot_rc,pattern_name(boot_pattern));
    printf("NO WRITE HAS RUN THIS BOOT\n\n");
    if(boot_rc || !boot_pattern) printf("STOP - FILE NOT RECOGNIZED\nSEND THIS SCREEN\n");
    else printf("A: WRITE A / NO FLUSH\nB: WRITE B / THEN FLUSH\n\nAFTER A: QUIT THEN POWER OFF/ON\nCHECK BOOT CONTENT BEFORE B\nDO NOT COPY FILES AGAIN\n");
    fflush(stdout);
}
static void progress50(enum step50 s) {
    title();
    const char *label=s==S_BIND?"BIND STATIC SLOT (NO OPEN)":s==S_WRITE?"WRITE512...":
        s==S_READ?"READ512 + BYTE COMPARE...":s==S_FLUSH?"FLUSH... (UP TO 60 SECONDS)":"COMPLETE";
    printf("%s\n",label);fflush(stdout);
}
static void result_page(const struct result50 *r,unsigned page) {
    title();printf("MODE %c  PAGE %u/2\n",r->mode==P_A?'A':'B',page+1);
    if(page==0) {
        printf("BOOT CONTENT: %s\n",pattern_name(boot_pattern));
        printf("BIND RC %d (NO OPEN)\n",r->bind_rc);
        if(r->attempted&2) printf("WRITE512 RC %d\n",r->write_rc); else printf("WRITE SKIPPED\n");
        if(r->attempted&4) printf("READ512 RC %d\n",r->read_rc); else printf("READ SKIPPED\n");
        if((r->attempted&4) && !r->read_rc) {
            printf("BYTE MATCH %u/512 %s\n",512-r->mismatches,r->mismatches?"FAIL":"PASS");
            if(r->mismatches) printf("FIRST BAD OFFSET %u\n",r->first_bad);
        } else printf("CONTENT NOT VERIFIED\n");
        if(r->attempted&8) printf("FLUSH RC %d\n",r->flush_rc);
        else printf("FLUSH NOT ISSUED\n");
        printf("RESULT %s RC %d\n",r->rc?"FAIL":"PASS",r->rc);
        printf("QUIT THEN POWER OFF/ON\nCHECK BOOT CONTENT %c VERIFIED\n",r->mode==P_A?'A':'B');
        printf("DO NOT COPY FILES AGAIN\n");
    } else {
        printf("FLUSH OBSERVATION\nSNAP RC %d\n",snap.rc);
        if(!(r->attempted&8)) printf("FLUSH WAS NOT ISSUED\n");
        else if(!forensic_header_valid(&snap)) printf("HEADER NOT VALIDATED\n");
        else {
            const uint32_t *s=snap.raw+40;
            printf("F8 RD %lu TX %lu\n",(unsigned long)s[0],(unsigned long)s[1]);
            printf("HOST WR %lu TABLE RD %lu\n",(unsigned long)s[2],(unsigned long)s[3]);
            printf("BUSY %lu DONE %lu\n",(unsigned long)s[7],(unsigned long)s[8]);
            printf("CMD %08lX -> %08lX\n",(unsigned long)s[24],(unsigned long)s[12]);
            printf("STATUS %08lX\n",(unsigned long)s[9]);
            printf("SLOT %lu SIZE SEEN %lu\n",(unsigned long)s[13],(unsigned long)(s[15]&4?1:0));
            printf("TX ABORT %lu LOST %lu\n",(unsigned long)s[17],(unsigned long)s[30]);
            printf("TX != HOST DECODE CONFIRMED\n");
        }
    }
    printf("\nLEFT/RIGHT PAGE  A SUMMARY\n");fflush(stdout);
}
#ifndef OF_PC
#include "of_file.h"
#include "of_input.h"
static uint32_t now50(void){return OF_SVC->timer_get_ms();}
static void idle50(void){usleep(1000);}
static int sdk_read512(uint8_t *out) {
    const struct read50_ops o={of_file_read_async,of_file_async_poll,of_file_async_busy,now50,idle50};
    return read512_run(&o,out);
}
static void stop50(const char *stage,int rc) {
    printf("STOP %s RC %d\nNO WRITE ISSUED\n",stage,rc);fflush(stdout);
    while(1)usleep(100000);
}
#endif
int main(int argc,char **argv) {
#ifdef OF_PC
    (void)sequence50;(void)read512_run;(void)boot_bytes;(void)progress50;(void)pattern_identify;
    if(argc>1 && !strcmp(argv[1],"--demo")) {
        for(unsigned mode=0;mode<5;++mode) {
            boot_rc=mode==4?-12:0;
            boot_pattern=mode==0?P_SEED:mode==1?P_A:mode==2?P_B:P_UNKNOWN;
            show_menu();
        }
        for(unsigned mode=0;mode<7;++mode) {
            struct result50 r={0};r.mode=mode?P_B:P_A;r.attempted=mode?15:7;
            r.flush_rc=mode==1?-12:0;r.rc=r.flush_rc;
            if(mode==2){r.attempted=7;r.mismatches=512;r.first_bad=0;r.rc=-1;}
            if(mode==3){r.attempted=3;r.write_rc=-2;r.rc=-2;}
            if(mode==4){r.attempted=7;r.read_rc=-12;r.rc=-12;}
            if(mode==5){r.attempted=1;r.bind_rc=-11;r.rc=-11;}
            memset(&snap,0,sizeof(snap));snap.words_read=72;snap.header_checked=1;
            snap.raw[0]=OF_BRIDGE_AUDIT_STREAM_MAGIC;
            for(unsigned i=40;i<72;++i)snap.raw[i]=9999999;
            result_page(&r,0);result_page(&r,1);
        }
        return 0;
    }
    int pass=of_static50_query()==OF_ERR_NOT_SUPPORTED && of_static50_bind()==OF_ERR_NOT_SUPPORTED;
    printf("Static50 unsupported PC gate: %s\n",pass?"PASS":"FAIL");return !pass;
#else
    (void)argc;(void)argv;title();fflush(stdout);
    if(!of_output_available() || !OF_OUTPUT_SVC_HAS(file_size) ||
       !OF_OUTPUT_SVC_HAS(timer_get_ms) || !OF_OUTPUT_SVC_HAS(input_poll_p0)) stop50("API",-8);
    int query=of_static50_query();
    if(query!=OF_STATIC50_MAGIC) stop50("WRONG OS",query);
    long size=OF_SVC->file_size("slot:4");
    if(size!=512) stop50("SLOT4 SIZE",(int)size);
    printf("BOOT READ512...\n");fflush(stdout);
    boot_rc=sdk_read512(boot_bytes);
    boot_pattern=boot_rc?P_UNKNOWN:pattern_identify(boot_bytes);
    show_menu();
    if(boot_rc || !boot_pattern) while(1)usleep(100000);
    /* Discard the launch button and require release before choosing a test. */
    do {of_input_poll_p0();usleep(16000);} while(of_btn(OF_BTN_A|OF_BTN_B));
    unsigned mode=0;
    while(!mode) {
        of_input_poll_p0();
        if(of_btn_pressed(OF_BTN_A))mode=P_A;
        else if(of_btn_pressed(OF_BTN_B))mode=P_B;
        usleep(16000);
    }
    int rc=of_bridge_audit_begin_with(of_bridge_trace_read);
    if(rc) stop50("AUDIT ARM",rc);
    const struct ops50 o={of_static50_bind,of_output_write,sdk_read512,of_output_flush,progress50};
    struct result50 r=sequence50(&o,mode);
    forensic_snapshot_read(&snap,of_bridge_trace_read);
    unsigned page=0;result_page(&r,page);
    /* One sequence per cold boot: no retry after an indeterminate command. */
    while(1) {
        of_input_poll_p0();unsigned prev=page;
        if(of_btn_pressed(OF_BTN_LEFT|OF_BTN_RIGHT))page^=1;
        if(of_btn_pressed(OF_BTN_A))page=0;
        if(page!=prev)result_page(&r,page);
        usleep(16000);
    }
#endif
}
