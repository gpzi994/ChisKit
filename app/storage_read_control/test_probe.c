#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "probe.h"
static unsigned checks;
#define CHECK(x) do { ++checks; if(!(x)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);exit(1);} } while(0)
static int available,open_result,flush_result,pre_busy,end_busy,start_result;
static int callback_result,wrong_token,duplicate,sync_callback,never_callback,poll_error,overrun;
static long os_size,post_size;
static uint32_t ticks,tick_step;
static unsigned opens,flushes,reads,starts,polls,busies,size_calls,progress_calls;
static void (*pending)(int,int);
static unsigned char *dest;
static void complete(void) {
    dest[0]=0;dest[1]=0x81;dest[2]=0xff;dest[3]=0x5a;
    if(overrun)dest[4]=0;
    pending(start_result+wrong_token,callback_result);
    if(duplicate)pending(start_result,callback_result);
}
static int start(int slot,uint32_t off,void *data,uint32_t len,void (*cb)(int,int)) {
    ++starts;
    CHECK(slot==4 && off==0 && len==4);
    CHECK(((uintptr_t)data&31u)==0);
    dest=data;pending=cb;
    if(sync_callback && start_result>=0)complete();
    return start_result;
}
static int poll(void) {
    ++polls;
    if(poll_error)return poll_error;
    if(!never_callback && polls==2){complete();return 1;}
    return 0;
}
static int busy(void){return ++busies==1?pre_busy:end_busy;}
static uint32_t now(void){return ticks;}
static void idle(void){ticks+=tick_step;}
static const struct read4_ops ops={start,poll,busy,now,idle};
static struct read4_result read4(void){++reads;return read4_run(&ops);}
static int avail(void){return available;}
static long size(const char *path) {
    ++size_calls;
    if(size_calls==1){CHECK(!strcmp(path,"slot:1"));return os_size;}
    CHECK(!strcmp(path,"slot:4"));return size_calls==2?64:post_size;
}
static int output_open(const char *path,uint32_t len) {
    ++opens;CHECK(!strcmp(path,FLUSH_PROBE_PATH));CHECK(len==1048613);return open_result;
}
static int flush(void){++flushes;CHECK(reads==1 && starts==1 && read4_callbacks==1);return flush_result;}
static void progress(enum read_control_stage stage,const struct read_control_result *r) {
    ++progress_calls;
    if(stage==RC_OPEN)CHECK(!r->open_attempted && !opens);
    if(stage==RC_READ4)CHECK(r->open_rc==0 && !r->read_attempted && !reads);
    if(stage==RC_FLUSH)CHECK(r->read.rc==0 && !r->flush_attempted && !flushes);
}
static const struct read_control_api api={avail,size,output_open,read4,flush};
static void reset(void) {
    available=1;os_size=137480;post_size=1048613;start_result=7;tick_step=100;
    open_result=flush_result=pre_busy=end_busy=callback_result=0;
    wrong_token=duplicate=sync_callback=never_callback=poll_error=overrun=0;
    ticks=opens=flushes=reads=starts=polls=busies=size_calls=progress_calls=0;
    pending=NULL;dest=NULL;
}
static struct read_control_result run(void){return read_control_run(&api,progress);}
static void expect_read_failure(int expected) {
    struct read_control_result r=run();
    CHECK(r.open_attempted && r.read_attempted && r.open_rc==0);
    CHECK(r.read.rc==expected);CHECK(!r.flush_attempted && !flushes);
    CHECK(opens==1 && reads==1 && starts<=1 && size_calls==3);
}
int main(void) {
    reset();struct read_control_result r=run();
    CHECK(r.open_rc==0 && r.read.rc==0 && r.flush_rc==0);
    CHECK(opens==1 && reads==1 && starts==1 && flushes==1 && polls==2);
    CHECK(r.read.callbacks==1 && r.read.callback_token==7 && r.read.token==7);
    CHECK(r.read.elapsed_ms==100 && r.read.polls==2);
    CHECK(r.read.bytes[0]==0 && r.read.bytes[1]==0x81 && r.read.bytes[2]==0xff && r.read.bytes[3]==0x5a);
    CHECK(progress_calls==3);
    reset();flush_result=OF_ERR_TIMEOUT;r=run();CHECK(r.flush_rc==-12 && flushes==1 && starts==1);
    reset();sync_callback=1;r=run();CHECK(r.read.rc==0 && !polls && flushes==1);
    reset();ticks=0xfffffff0u;r=run();CHECK(r.read.rc==0 && r.read.elapsed_ms==100);
    reset();pre_busy=1;expect_read_failure(OF_ERR_BUSY);CHECK(!starts);
    reset();pre_busy=OF_ERR_NOT_SUPPORTED;expect_read_failure(OF_ERR_NOT_SUPPORTED);CHECK(!starts);
    reset();start_result=OF_ERR_BUSY;expect_read_failure(OF_ERR_BUSY);CHECK(starts==1 && !polls);
    reset();start_result=OF_ERR_NOT_SUPPORTED;expect_read_failure(OF_ERR_NOT_SUPPORTED);
    reset();callback_result=OF_ERR_TIMEOUT;expect_read_failure(OF_ERR_TIMEOUT);
    reset();callback_result=OF_ERR_IO;expect_read_failure(OF_ERR_IO);
    reset();callback_result=1;expect_read_failure(1);
    reset();wrong_token=1;expect_read_failure(OF_ERR_FAILED);
    reset();duplicate=1;expect_read_failure(OF_ERR_FAILED);
    reset();end_busy=1;expect_read_failure(OF_ERR_BUSY);
    reset();end_busy=OF_ERR_FAILED;expect_read_failure(OF_ERR_FAILED);
    reset();overrun=1;expect_read_failure(OF_ERR_BAD_RANGE);
    reset();poll_error=OF_ERR_INVALID_STATE;expect_read_failure(OF_ERR_INVALID_STATE);
    reset();poll_error=OF_ERR_IO;expect_read_failure(OF_ERR_IO);
    reset();never_callback=1;expect_read_failure(OF_ERR_TIMEOUT);CHECK(polls==51);
    /* A late completion writes persistent storage; it cannot restart the
     * sequence or issue a FLUSH after the failed test has returned. */
    complete();CHECK(!flushes && read4_callbacks==1);
    reset();never_callback=1;tick_step=0;expect_read_failure(OF_ERR_TIMEOUT);CHECK(polls==10000);
    reset();available=0;r=run();CHECK(!opens && !reads && !flushes && !size_calls);
    reset();os_size=137479;r=run();CHECK(!opens && !reads && !flushes && size_calls==1);
    reset();open_result=OF_ERR_IO;r=run();CHECK(opens==1 && !reads && !flushes && size_calls==2);
    reset();post_size=-1;r=run();CHECK(opens==1 && !reads && !flushes && size_calls==3);
    reset();post_size=64;r=run();CHECK(!r.read_attempted && !r.flush_attempted);
    printf("Read4 control: %u checks PASS; single 4-byte SDK request; no writes/retries\n",checks);
    return 0;
}
