#include <stdio.h>
#include <stdlib.h>
#include "snapshot.h"
static unsigned checks;
#define CHECK(x) do { ++checks; if(!(x)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);exit(1);} } while(0)
static uint32_t stream[FORENSIC_MAX_WORDS],flags,ready_value;
static unsigned calls,polls,cursor,fail_at,ready_after;
static int failure_rc;
static struct {
    uint32_t before;
    struct forensic_snapshot s;
    uint32_t after;
} guarded;
static int reader(unsigned reg,uint32_t *out) {
    ++calls;
    if(calls==fail_at) {
        /* A failed read is allowed to have side effects. Do not retry or
         * publish its poisoned output as one successfully read word. */
        if(reg==OF_BRIDGE_TRACE_DATA) ++cursor;
        *out=0xdeadbeefu;return failure_rc;
    }
    if(reg==OF_BRIDGE_TRACE_FREEZE) {*out=++polls>=ready_after?ready_value:2;return 0;}
    if(reg==OF_BRIDGE_TRACE_COUNT) {*out=flags;return 0;}
    if(reg==OF_BRIDGE_TRACE_LEGACY) {*out=0xdf3f1c60u;return 0;}
    CHECK(reg==OF_BRIDGE_TRACE_DATA && cursor<FORENSIC_MAX_WORDS);
    *out=stream[cursor++];return 0;
}
static void reset(unsigned count) {
    calls=polls=cursor=fail_at=0;ready_after=3;ready_value=1;failure_rc=-1;
    flags=0x40000u|count;
    for(unsigned i=0;i<FORENSIC_MAX_WORDS;++i)stream[i]=0x80000000u+i;
    stream[0]=OF_BRIDGE_AUDIT_STREAM_MAGIC;stream[1]=1;stream[2]=72;
    stream[3]=count;stream[5]=flags&0xffff0000u;stream[6]=1;stream[7]=0;
    memset(&guarded,0xa5,sizeof(guarded));
    guarded.before=0x12345678;guarded.after=0x9abcdef0;
}
static int run(void) {
    int rc=forensic_snapshot_read(&guarded.s,reader);
    CHECK(guarded.before==0x12345678 && guarded.after==0x9abcdef0);
    CHECK(guarded.s.calls==calls && guarded.s.words_read<=FORENSIC_MAX_WORDS);
    for(unsigned i=0;i<guarded.s.words_read;++i)CHECK(guarded.s.raw[i]==stream[i]);
    for(unsigned i=guarded.s.words_read;i<FORENSIC_MAX_WORDS;++i)CHECK(guarded.s.raw[i]==0);
    return rc;
}
int main(void) {
    struct forensic_snapshot *s=&guarded.s;
    const unsigned counts[]={0,1,37,42,256};
    for(unsigned n=0;n<sizeof(counts)/sizeof(counts[0]);++n) {
        unsigned count=counts[n];reset(count);CHECK(run()==0);
        CHECK(s->stage==FS_DONE && !s->api_rc && s->flags_read && s->legacy_read);
        CHECK(s->count_valid && s->words_read==72+4*count && cursor==72+4*count);
        CHECK(s->expected_words==s->words_read && s->header_checked && !s->bad_mask);
        CHECK(forensic_raw_groups(s)==count && forensic_header_valid(s));
        CHECK(s->failure_word==FORENSIC_NO_WORD && s->first_bad==FORENSIC_NO_WORD);
        CHECK(s->legacy==0xdf3f1c60u && polls==3);
    }
    /* Every individual service failure in a maximum-size snapshot, including
     * all 72 header words and all 1024 payload words. */
    for(unsigned fail=1;fail<=5+FORENSIC_MAX_WORDS;++fail) {
        reset(256);fail_at=fail;failure_rc=(fail&1)?-1:-2;
        CHECK(run()==failure_rc && s->api_rc==failure_rc && calls==fail);
        if(fail<=3)CHECK(s->stage==FS_FREEZE && !s->flags_read && !s->words_read);
        else if(fail==4)CHECK(s->stage==FS_COUNT && !s->flags_read && !s->words_read);
        else if(fail==5)CHECK(s->stage==FS_LEGACY && s->flags_read && !s->legacy_read);
        else {
            unsigned index=fail-6;
            CHECK(s->failure_word==index && s->words_read==index && cursor==index+1);
            CHECK(s->stage==(index<72?FS_HEADER:FS_PAYLOAD));
            CHECK(s->header_checked==(index>=72));
            CHECK(forensic_raw_groups(s)==(index<72?0:(index-72)/4));
        }
    }
    /* Preserve the full bounded stream on all six legacy header checks. */
    const unsigned fields[]={0,1,2,3,5,6};
    for(unsigned i=0;i<sizeof(fields)/sizeof(fields[0]);++i) {
        unsigned h=fields[i];reset(37);stream[h]^=0x100;
        CHECK(run()==OF_ERR_FAILED && s->api_rc==0);
        CHECK(s->stage==FS_HEADER_CHECK && s->first_bad==h && s->bad_mask==(1u<<h));
        CHECK(s->words_read==220 && cursor==220 && forensic_raw_groups(s)==37);
        CHECK(!forensic_header_valid(s) && s->failure_word==FORENSIC_NO_WORD);
    }
    reset(37);stream[2]=stream[3]=0xffffffffu;
    CHECK(run()==-1 && s->words_read==220 && s->expected_words==220);
    CHECK(s->bad_mask==12 && s->first_bad==2 && cursor==220);
    reset(37);stream[0]=0;fail_at=5+72+7;failure_rc=-17;
    CHECK(run()==-17 && s->stage==FS_PAYLOAD && s->failure_word==78);
    CHECK(s->first_bad==0 && s->bad_mask==1 && s->words_read==78);
    CHECK(forensic_raw_groups(s)==1 && !forensic_header_valid(s));
    /* Empty captures and arbitrary unvalidated fields are not false errors. */
    reset(0);stream[4]=stream[7]=0xffffffffu;stream[6]=0;
    CHECK(run()==0 && forensic_header_valid(s));
    reset(256);flags|=0xb0000;stream[5]=flags&0xffff0000u;stream[6]=2;
    CHECK(run()==0 && s->flags==0xf0100 && s->words_read==FORENSIC_MAX_WORDS);
    reset(37);ready_value=0x80000001u;CHECK(run()==0 && s->ready==ready_value);
    /* Invalid count prevents all DATA access. No allocation uses a raw count. */
    const uint32_t invalid[]={0x40101u,0x4ffffu,0x80040025u};
    for(unsigned i=0;i<sizeof(invalid)/sizeof(invalid[0]);++i) {
        reset(0);flags=invalid[i];CHECK(run()==-1 && !s->api_rc);
        CHECK(s->flags_read && s->flags==invalid[i] && !s->count_valid);
        CHECK(s->stage==FS_COUNT_CHECK && calls==4 && !cursor && !s->words_read);
    }
    reset(37);ready_after=10001;CHECK(run()==OF_ERR_TIMEOUT && !s->api_rc);
    CHECK(s->stage==FS_FREEZE && s->freeze_polls==10000 && calls==10000 && !cursor);
    reset(37);fail_at=6;failure_rc=9;
    CHECK(run()==9 && s->api_rc==9 && s->failure_word==0 && !s->words_read);
    CHECK(forensic_snapshot_read(NULL,reader)==OF_ERR_INVALID_PARAM);
    reset(37);CHECK(forensic_snapshot_read(s,NULL)==OF_ERR_INVALID_PARAM && !calls);
    CHECK(s->stage==FS_NONE && s->failure_word==FORENSIC_NO_WORD && !s->words_read);
    CHECK(!strcmp(forensic_stage_name(FS_HEADER_CHECK),"HEADER-CHECK"));
    CHECK(!strcmp(forensic_stage_name(FS_PAYLOAD),"PAYLOAD-READ"));
    printf("Snapshot forensics checks: %u PASS\n",checks);
    return 0;
}
