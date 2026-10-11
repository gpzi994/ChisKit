#ifndef STORAGE_SLOT_PROBE_H
#define STORAGE_SLOT_PROBE_H

#include "../storage_flush_probe/probe.h"

struct slot_probe_api {
    int (*available)(void);
    long (*size)(const char *);
    int (*open)(const char *, uint32_t);
    int (*flush)(void);
};
struct slot_probe_result {
    int api_ok, open_attempted, open_rc, flush_attempted, flush_rc;
    long os_size, slot_pre, slot_post;
};

static inline struct slot_probe_result slot_probe_run(
        const struct slot_probe_api *api,
        void (*before_open)(const struct slot_probe_result *),
        void (*before_flush)(const struct slot_probe_result *)) {
    struct slot_probe_result r = {
        0, 0, OF_ERR_INVALID_STATE, 0, OF_ERR_INVALID_STATE, -1, -1, -1
    };
    r.api_ok = api->available() == 1;
    if (!r.api_ok)
        return r;
    /* Read through existing SDK services, never application MMIO. Slot 1 is
     * a known, fixed-size OS image: do not issue output commands if this
     * metadata sanity check fails. Both A/B packages use the same OS/RBF. */
    r.os_size = api->size("slot:1");
    if (r.os_size != 137480L)
        return r;
    r.slot_pre = api->size("slot:4");
    if (before_open)
        before_open(&r);
    r.open_attempted = 1;
    r.open_rc = api->open(FLUSH_PROBE_PATH, FLUSH_PROBE_SIZE);
    if (r.open_rc != OF_OK)
        return r;
    r.slot_post = api->size("slot:4");
    if (before_flush)
        before_flush(&r);
    /* A negative size is reported as unresolved, not as proof of a missing
     * entry. Still run the same one-shot FLUSH to compare A with B. */
    r.flush_attempted = 1;
    r.flush_rc = api->flush();
    return r; /* no retry, query or file command after FLUSH */
}
#endif
