#include <stdio.h>
#include <unistd.h>
#include "of_output.h"
#include "probe.h"
#include "../storage_slot_probe/probe.h"
#ifndef OF_PC
#include "of_input.h"
static struct bridge_snapshot snapshot;
static int api_available(void) { return of_output_available() && OF_OUTPUT_SVC_HAS(file_size); }
static long slot_size(const char *p) { return OF_SVC->file_size(p); }
static void pre_open(const struct slot_probe_result *r) {
    printf("OS SLOT1: %ld\nSLOT4 PRE: %ld\nOPEN...\n",r->os_size,r->slot_pre); fflush(stdout);
}
static void pre_flush(const struct slot_probe_result *r) {
    printf("OPEN RC=%d\nSLOT4 POST: %ld\nFLUSH NO WRITE...\n",r->open_rc,r->slot_post); fflush(stdout);
}
static void show_page(unsigned page,const struct slot_probe_result *r) {
    unsigned pages=1+(snapshot.count+5)/6;
    printf("\033[2J\033[H TF TRACE smoke.46  %u/%u\n",page+1,pages);
    if(page==0) {
        unsigned rd_table=0, wr_status=0;
        uint32_t slot=0, size=0, id_addr=0; int have_slot=0,have_size=0;
        for(unsigned i=0;i<snapshot.count;++i) {
            struct bridge_event *e=&snapshot.events[i]; unsigned kind=e->meta>>28;
            uint32_t addr=e->address&0xff00ffffu;
            if(kind==1 && (addr&0xfffff000u)==0xf8002000u) ++rd_table;
            if(kind==3 && addr==0xf8001000u) ++wr_status;
            if(kind==2 && addr==0xf8001020u) {slot=e->data; have_slot=1;}
            if(kind==2 && addr>=0xf8002000u && addr<=0xf80020ffu) {
                if(!(addr&7) && (e->data&0xffff)==4) id_addr=addr;
                if(id_addr && addr==id_addr+4) {size=e->data; have_size=1;}
            }
        }
        printf("KEEP CARTRIDGE EMPTY\nOPEN %d   FLUSH %d\n",r->open_rc,r->flush_rc);
        printf("POST %ld\nST %08lX\n",r->slot_post,(unsigned long)snapshot.legacy);
        printf("SNAP RC %d  N %lu\n",snapshot.rc,(unsigned long)snapshot.count);
        printf("FLAGS %08lX\n",(unsigned long)snapshot.count_flags);
        printf("SLOT TX %s %08lX\n",have_slot?"YES":"NO",(unsigned long)slot);
        printf("SIZE TX %s %lu\n",have_size?"YES":"NO",(unsigned long)size);
        printf("TABLE RD %u  STATUS WR %u\n",rd_table,wr_status);
        printf("FPGA TX != HOST CONFIRM\n");
    } else {
        unsigned first=(page-1)*6;
        printf("# TYPE TICKS / ADDR DATA NEXT\n");
        for(unsigned i=first;i<snapshot.count && i<first+6;++i) {
            const struct bridge_event *e=&snapshot.events[i];
            const char *type=e->meta>>28==1?"RD":e->meta>>28==2?"TX":e->meta>>28==3?"WR":e->meta>>28==4?"?T":"AR";
            printf("%03u %s %07lX\n%08lX %08lX %08lX\n",i,type,(unsigned long)(e->meta&0xfffffff),
                (unsigned long)e->address,(unsigned long)e->data,(unsigned long)e->current);
        }
    }
    printf("\nLEFT/RIGHT: PAGE   A: SUMMARY\n"); fflush(stdout);
}
#endif
int main(void) {
#ifdef OF_PC
    uint32_t id=0;
    int pass=of_bridge_trace_read(0,&id)==OF_ERR_NOT_SUPPORTED;
    printf("PC unsupported fallback: %s\n",pass?"PASS":"FAIL"); return !pass;
#else
    printf("\033[2J\033[H TF TRACE smoke.46\nKEEP CARTRIDGE EMPTY\n"); fflush(stdout);
    uint32_t id=0;
    int rc=of_bridge_trace_read(OF_BRIDGE_TRACE_ID,&id);
    if(rc || id!=OF_BRIDGE_TRACE_MAGIC) {
        printf("TRACE BUILD MISMATCH\nRC %d ID %08lX\nNO FILE COMMANDS ISSUED\n",rc,(unsigned long)id);
        fflush(stdout); while(1) usleep(100000);
    }
    const struct slot_probe_api api={api_available,slot_size,of_output_open,of_output_flush};
    struct slot_probe_result r=slot_probe_run(&api,pre_open,pre_flush);
    read_snapshot(&snapshot,of_bridge_trace_read);
    if(snapshot.rc) snapshot.count=0; // never paginate partial/invalid data
    unsigned page=0,pages=1+(snapshot.count+5)/6;
    show_page(page,&r);
    while(1) {
        of_input_poll_p0();
        unsigned old=page;
        if(of_btn_pressed(OF_BTN_RIGHT)) page=(page+1)%pages;
        if(of_btn_pressed(OF_BTN_LEFT)) page=(page+pages-1)%pages;
        if(of_btn_pressed(OF_BTN_A)) page=0;
        if(page!=old) show_page(page,&r);
        usleep(16000);
    }
#endif
}
