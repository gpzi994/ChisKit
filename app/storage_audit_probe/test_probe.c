#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "probe.h"
static uint32_t stream[72+1024],flags,id;
static unsigned calls,polls,words,fail_at,ready_after;
static int reader(unsigned reg,uint32_t *out){
    ++calls;if(calls==fail_at)return OF_ERR_FAILED;
    if(reg==OF_BRIDGE_TRACE_ID){*out=id;return 0;}
    if(reg==OF_BRIDGE_TRACE_FREEZE){*out=(++polls>=ready_after);return 0;}
    if(reg==OF_BRIDGE_TRACE_COUNT){*out=flags;return 0;}
    if(reg==OF_BRIDGE_TRACE_LEGACY){*out=0xdf3f1c60;return 0;}
    assert(reg==OF_BRIDGE_TRACE_DATA && words<72+1024);*out=stream[words++];return 0;
}
static void reset(unsigned count){
    calls=polls=words=fail_at=0;ready_after=3;flags=0x40000u|count;id=OF_BRIDGE_AUDIT_MAGIC;
    for(unsigned i=0;i<72+1024;++i)stream[i]=0x80000000u+i;
    stream[0]=OF_BRIDGE_AUDIT_STREAM_MAGIC;stream[1]=1;stream[2]=72;
    stream[3]=count;stream[5]=flags&0xffff0000u;stream[6]=2;
}
int main(void){
    struct audit_snapshot s;memset(&s,0,sizeof(s));
    reset(2);assert(audit_snapshot_read(&s,reader)==0);assert(s.count==2&&words==80&&polls==3);
    assert(s.event[1].word[3]==0x8000004fu&&s.legacy==0xdf3f1c60u);
    reset(256);flags|=0x30000;stream[5]=flags&0xffff0000u;
    assert(audit_snapshot_read(&s,reader)==0&&s.count==256&&words==1096);
    reset(0);assert(audit_snapshot_read(&s,reader)==0&&words==72&&!s.count);
    reset(257);assert(audit_snapshot_read(&s,reader)==OF_ERR_FAILED&&!words);
    reset(2);ready_after=10001;assert(audit_snapshot_read(&s,reader)==OF_ERR_TIMEOUT&&!words);
    for(unsigned fail=1;fail<=85;++fail){
        reset(2);fail_at=fail;assert(audit_snapshot_read(&s,reader)==OF_ERR_FAILED&&calls==fail&&!s.count);
    }
    for(unsigned index=0;index<7;++index){
        if(index==4)continue;
        reset(2);stream[index]^=0x100;assert(audit_snapshot_read(&s,reader)==OF_ERR_FAILED&&!s.count);
    }
    reset(0);flags=0x80000000u;assert(of_bridge_audit_begin_with(reader)==0&&calls==2);
    reset(0);id=OF_BRIDGE_TRACE_MAGIC;assert(of_bridge_audit_begin_with(reader)==OF_ERR_INVALID_STATE&&calls==1);
    reset(0);assert(of_bridge_audit_begin_with(reader)==OF_ERR_TIMEOUT&&calls==10001);
    reset(0);fail_at=2;assert(of_bridge_audit_begin_with(reader)==OF_ERR_FAILED&&calls==2);
    puts("Audit SDK mock cases: 100 PASS");
}
