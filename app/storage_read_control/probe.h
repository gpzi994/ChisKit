#ifndef STORAGE_READ_CONTROL_H
#define STORAGE_READ_CONTROL_H
#include "read4.h"
#include "../storage_flush_probe/probe.h"
enum read_control_stage { RC_OPEN, RC_READ4, RC_FLUSH };
struct read_control_result {
    int api_ok, open_attempted, open_rc, read_attempted, flush_attempted, flush_rc;
    long os_size, slot_pre, slot_post;
    struct read4_result read;
};
struct read_control_api {
    int (*available)(void);
    long (*size)(const char *);
    int (*open)(const char *,uint32_t);
    struct read4_result (*read4)(void);
    int (*flush)(void);
};
static struct read_control_result read_control_run(const struct read_control_api *api,
        void (*progress)(enum read_control_stage,const struct read_control_result *)) {
    struct read_control_result r={0};
    r.open_rc=r.flush_rc=r.read.rc=OF_ERR_INVALID_STATE;
    r.os_size=r.slot_pre=r.slot_post=-1;
    r.api_ok=api->available()==1;
    if (!r.api_ok) return r;
    r.os_size=api->size("slot:1");
    if (r.os_size!=137480L) return r;
    r.slot_pre=api->size("slot:4");
    if (progress) progress(RC_OPEN,&r);
    r.open_attempted=1;
    r.open_rc=api->open(FLUSH_PROBE_PATH,FLUSH_PROBE_SIZE);
    if (r.open_rc!=OF_OK) return r;
    r.slot_post=api->size("slot:4");
    if (r.slot_post!=(long)FLUSH_PROBE_SIZE) return r;
    if (progress) progress(RC_READ4,&r);
    r.read_attempted=1;
    r.read=api->read4();
    if (r.read.rc!=OF_OK) return r;
    if (progress) progress(RC_FLUSH,&r);
    r.flush_attempted=1;
    r.flush_rc=api->flush();
    return r; /* No retry or file command after FLUSH, on any outcome. */
}
#endif
