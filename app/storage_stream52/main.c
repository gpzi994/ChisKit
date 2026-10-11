#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "of_static_output52.h"
#include "stream.h"
#include "read_block.h"
static struct guard51 gate;
static struct scan52 boot;
static unsigned read_calls,write_calls,bind_calls;
static const char *kind52(unsigned kind) {
    return kind==INITIAL52?"INITIAL":kind==DATA52?"DATA VERIFIED":"UNKNOWN";
}
static const char *stage52(enum stage52 stage) {
    switch(stage) {
    case ST52_BOOT:return "BOOT READ";
    case ST52_BIND:return "BIND (NO HOST OPEN)";
    case ST52_WRITE:return "WRITE";
    case ST52_VERIFY:return "VERIFY READ";
    default:return "COMPLETE";
    }
}
static void title52(void){printf("\033[2J\033[H TF STREAM smoke.52\nKEEP CARTRIDGE EMPTY\n");}
static void progress52(enum stage52 stage,unsigned blocks) {
    title52();printf("%s %u/256 BLOCKS\n",stage52(stage),blocks);
    printf("FILE static52.bin  SIZE 1 MiB\nBLOCK SIZE 4096 BYTES\n");
    printf("BIND %u WRITE %u READ %u\n",bind_calls,write_calls,read_calls);
    printf("FLUSH: NOT USED\nPLEASE WAIT; DO NOT POWER OFF\n");fflush(stdout);
}
static void show_boot52(void) {
    title52();printf("FILE static52.bin  SIZE 1 MiB\nBOOT CONTENT: %s\n",kind52(boot.kind));
    printf("BOOT RC %d BLOCKS %u/256\n",boot.rc,boot.blocks);
    printf("BYTES CHECKED %u/1048576\n",boot.bytes);
    printf("%sCRC %08lX\n",boot.bytes==TOTAL52?"":"PARTIAL ",(unsigned long)boot.crc);
    printf("EXPECTED I %08lX D %08lX\n",(unsigned long)CRC_INITIAL52,(unsigned long)CRC_DATA52);
    printf("HEAD:");for(unsigned i=0;i<8;++i)printf(" %02X",boot.head[i]);printf("\n");
    printf("WRITE CALLS %u FLUSH NOT USED\n",write_calls);
    printf("BIND %u READ %u KEY %04lX\n",bind_calls,read_calls,(unsigned long)gate.keys);
    if(!valid_boot52(&boot)) {
        printf("WRITE LOCK: DISABLED\nFIRST BAD I %u D %u\n",boot.first_initial,boot.first_data);
        printf("SEND THIS SCREEN; DO NOT RETRY\n");
    } else if(boot.kind==DATA52) {
        printf("WRITE LOCK: DISABLED\nFULL FILE MATCHES WRITTEN DATA\nSEND THIS SCREEN; NO REWRITE\n");
    } else switch(gate.state) {
    case G_QUIET:printf("WRITE LOCK: LOCKED\nRELEASE ALL FOR 0.5 SECOND\nNO AUTOMATIC WRITE\n");break;
    case G_LOCKED:printf("WRITE LOCK: LOCKED\nL+R HOLD 2 SEC TO UNLOCK\nTHEN A; RELEASE; HOLD START\n");break;
    case G_HOLD_LR:printf("WRITE LOCK: LOCKED\nKEEP L+R HELD FOR 2 SECONDS\nNO WRITE YET\n");break;
    case G_RELEASE_LR:printf("UNLOCKED; NOT CONFIRMED\nRELEASE ALL FOR 0.5 SECOND\nNO WRITE YET\n");break;
    case G_CHOOSE:printf("A: SELECT 1 MiB WRITE\nB: CANCEL; NO B/FLUSH MODE\nCONFIRM WITHIN 10 SECONDS\n");break;
    case G_RELEASE_MODE:printf("SELECTED WRITE; NOT CONFIRMED\nRELEASE ALL FOR 0.5 SECOND\nNO WRITE YET\n");break;
    case G_CONFIRM:printf("CONFIRM 1 MiB TEST FILE WRITE\nHOLD START (+) FOR 1 SECOND\nOTHER KEY CANCELS\n");break;
    case G_HOLD_START:printf("CONFIRMING 1 MiB WRITE\nKEEP START (+) HELD 1 SECOND\nRELEASE EARLY CANCELS\n");break;
    case G_DONE:printf("ONE SHOT CONFIRMED\nNO RETRY THIS BOOT\n\n");break;
    }
    printf("BOOT MS %lu\n",(unsigned long)boot.ms);fflush(stdout);
}
static void show_result52(const struct job52 *r) {
    title52();printf("STREAM %s RC %d\n",r->rc?"FAIL":"PASS",r->rc);
    printf("STAGE %s\n",stage52(r->stage));
    printf("BIND RC %d\nWRITE RC %d BLOCKS %u/256\n",r->bind_rc,r->write_rc,r->write_done);
    printf("VERIFY RC %d BLOCKS %u/256\n",r->verify.rc,r->verify.blocks);
    printf("CHECKED %u/1048576 BAD %u\n",r->verify.bytes,r->verify.bad_data);
    printf("%sCRC %08lX EXPECT %08lX\n",r->verify.bytes==TOTAL52?"":"PARTIAL ",
           (unsigned long)r->verify.crc,(unsigned long)CRC_DATA52);
    if(r->failed_offset==TOTAL52)printf("FIRST BAD: NONE\n");
    else printf("STOP BYTE %u / %08lX\n",r->failed_offset,(unsigned long)r->failed_offset);
    printf("WRITE CALLS %u READ %u\n",write_calls,read_calls);
    printf("FLUSH: NOT USED\nELAPSED MS %lu\n",(unsigned long)r->total_ms);
    if(!r->rc)printf("PERSISTENCE: NOT YET TESTED\nQUIT; POWER OFF/ON; OPEN 52\nDO NOT RECOPY static52.bin\n");
    else printf("STOP; SEND SCREEN; NO RETRY\nPARTIAL FILE MAY REMAIN\n");
    fflush(stdout);
}
#ifndef OF_PC
#include "of_file.h"
#include "of_input.h"
static uint32_t now52(void){return OF_SVC->timer_get_ms();}
static void idle52(void){usleep(1000);}
static int sdk_read52(uint32_t off,uint8_t *out) {
    const struct read_ops52 ops={of_file_read_async,of_file_async_poll,of_file_async_busy,now52,idle52};
    ++read_calls;return read_block52(&ops,off,out);
}
static int sdk_bind52(void){++bind_calls;return of_static52_bind();}
static int sdk_write52(uint32_t off,const void *src,uint32_t n) {
    ++write_calls;return of_output_write(off,src,n);
}
static void halt52(void){fflush(stdout);while(1)usleep(100000);}
#endif
int main(int argc,char **argv) {
    /* The reused gate header also declares legacy pattern helpers. */
    (void)fill51;(void)identify51;(void)crc51;
#ifdef OF_PC
    (void)run_stream52;(void)gate_tick52;(void)read_block52;
    if(argc>1 && !strcmp(argv[1],"--demo")) {
        boot=(struct scan52){.kind=INITIAL52,.blocks=256,.bytes=TOTAL52,.crc=CRC_INITIAL52,
            .first_initial=TOTAL52,.first_data=6,.ms=12345};
        fill52(boot.head,0,8,INITIAL52);read_calls=256;
        for(unsigned state=G_QUIET;state<=G_DONE;++state){gate.state=state;show_boot52();}
        boot.kind=DATA52;boot.crc=CRC_DATA52;fill52(boot.head,0,8,DATA52);show_boot52();
        boot.rc=-12;boot.blocks=255;boot.bytes=255*CHUNK52;boot.kind=UNKNOWN52;show_boot52();
        struct job52 r={.stage=ST52_DONE,.write_done=256,.failed_offset=TOTAL52,.total_ms=999999};
        r.verify=(struct scan52){.kind=DATA52,.bytes=TOTAL52,.blocks=256,.crc=CRC_DATA52};
        write_calls=256;read_calls=512;show_result52(&r);
        r.stage=ST52_VERIFY;r.rc=-1;r.verify.rc=-1;r.verify.bad_data=CHUNK52;
        r.failed_offset=1048575;show_result52(&r);
        r.verify.bytes=CHUNK52;r.verify.blocks=1;r.rc=r.verify.rc=-12;show_result52(&r);
        r.stage=ST52_WRITE;r.write_rc=-2;r.verify.rc=-10;r.verify.bytes=0;r.write_done=0;
        r.verify.blocks=0;show_result52(&r);
        for(unsigned s=ST52_BOOT;s<=ST52_DONE;++s){progress52(s,0);progress52(s,256);}
        return 0;
    }
    int ok=of_static52_query()==OF_ERR_NOT_SUPPORTED && of_static52_bind()==OF_ERR_NOT_SUPPORTED;
    printf("Stream52 PC service gate: %s\n",ok?"PASS":"FAIL");return !ok;
#else
    (void)argc;(void)argv;title52();
    if(!OF_OUTPUT_SVC_HAS(output_open) || !OF_OUTPUT_SVC_HAS(output_write) ||
       !OF_OUTPUT_SVC_HAS(file_size) || !OF_OUTPUT_SVC_HAS(timer_get_ms) ||
       !OF_OUTPUT_SVC_HAS(input_poll_p0)) {
        printf("STOP API MISSING; NO WRITE\n");halt52();
    }
    int query=of_static52_query();
    if(query!=OF_STATIC52_MAGIC){printf("STOP WRONG OS %08lX\nCOPY ALL FILES; POWER OFF/ON\n",(unsigned long)query);halt52();}
    long size=OF_SVC->file_size("slot:4");
    if(size!=TOTAL52){printf("STOP SLOT4 SIZE %ld\nEXPECTED 1048576; NO WRITE\n",size);halt52();}
    const struct ops52 ops={sdk_bind52,sdk_write52,sdk_read52,now52,progress52};
    boot=scan_file52(&ops,0,ST52_BOOT);show_boot52();
    /* A written or damaged fixture is inspection-only, never automatically
     * rewritten. Recopying a seed is an explicit separate user action. */
    if(!valid_boot52(&boot) || boot.kind!=INITIAL52)halt52();
    while(1) {
        of_input_poll_p0();enum gate51 old=gate.state;
        if(gate_tick52(&gate,_of_input_p0.buttons,now52()))break;
        if(old!=gate.state)show_boot52();
        usleep(16000);
    }
    struct job52 r=run_stream52(&gate,&boot,&ops);show_result52(&r);halt52();
#endif
}
