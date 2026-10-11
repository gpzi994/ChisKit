#ifndef STORAGE_FLUSH_PROBE_H
#define STORAGE_FLUSH_PROBE_H

#include <stdint.h>
#include "of_error.h"

/* Keep the same diagnostic path and requested size as smoke.42. The only
 * file-operation change is omission of ALL output_write calls. */
#define FLUSH_PROBE_PATH "/Saves/chisburner/common/storage-smoke.41.bin"
#define FLUSH_PROBE_SIZE (1024u * 1024u + 37u)

struct flush_probe_api {
    int (*available)(void);
    int (*open)(const char *, uint32_t);
    int (*flush)(void);
};

struct flush_probe_result {
    int api_ok;
    int open_rc;
    int flush_rc;
    int flush_attempted;
};

static inline struct flush_probe_result flush_probe_run(
        const struct flush_probe_api *api,
        void (*opened)(const struct flush_probe_result *)) {
    struct flush_probe_result r = {
        0, OF_ERR_NOT_SUPPORTED, OF_ERR_INVALID_STATE, 0
    };
    r.api_ok = api->available() == 1;
    if (!r.api_ok)
        return r;
    r.open_rc = api->open(FLUSH_PROBE_PATH, FLUSH_PROBE_SIZE);
    if (opened)
        opened(&r);
    if (r.open_rc != OF_OK)
        return r;
    r.flush_attempted = 1;
    r.flush_rc = api->flush();
    /* In particular, do not retry or issue another file command on timeout:
     * the hardware request might still be in flight. */
    return r;
}

#endif
