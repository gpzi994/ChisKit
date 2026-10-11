#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "probe.h"
static unsigned polls, words, calls, fail_at, ready_after;
static uint32_t flags;
static int mock_read(unsigned word,uint32_t *out) {
    ++calls;
    if(calls==fail_at) return OF_ERR_FAILED;
    if(word==OF_BRIDGE_TRACE_FREEZE) { *out=(++polls>=ready_after); return 0; }
    if(word==OF_BRIDGE_TRACE_COUNT) { *out=flags; return 0; }
    if(word==OF_BRIDGE_TRACE_LEGACY) { *out=0xdf3f1c60u; return 0; }
    assert(word==OF_BRIDGE_TRACE_DATA); *out=0x80000000u+words++; return 0;
}
static void reset(unsigned count) {
    polls=words=calls=fail_at=0; ready_after=3; flags=0x40000u|count;
}
int main(void) {
    struct bridge_snapshot s; memset(&s,0,sizeof(s));
    reset(2); assert(read_snapshot(&s,mock_read)==0);
    assert(polls==3 && words==8 && s.count==2 && s.legacy==0xdf3f1c60u);
    assert(s.events[0].meta==0x80000000u && s.events[0].current==0x80000003u);
    assert(s.events[1].address==0x80000005u && s.events[1].data==0x80000006u);
    reset(0); assert(read_snapshot(&s,mock_read)==0 && !words && !s.count);
    reset(256); flags|=0x30000; assert(read_snapshot(&s,mock_read)==0);
    assert(words==1024 && s.count_flags==0x70100u && s.events[255].current==0x800003ffu);
    reset(257); assert(read_snapshot(&s,mock_read)==OF_ERR_FAILED && !words);
    reset(2); ready_after=10001;
    assert(read_snapshot(&s,mock_read)==OF_ERR_TIMEOUT && polls==10000 && !words);
    for(unsigned fail=1;fail<=13;++fail) {
        reset(2); fail_at=fail;
        assert(read_snapshot(&s,mock_read)==OF_ERR_FAILED && calls==fail);
    }
    assert(of_bridge_trace_read(0,NULL)==OF_ERR_INVALID_PARAM);
    uint32_t v=1; assert(of_bridge_trace_read(5,&v)==OF_ERR_INVALID_PARAM);
    assert(of_bridge_trace_read(0,&v)==OF_ERR_NOT_SUPPORTED && v==0);
    puts("Snapshot mock cases: 21 PASS");
}
