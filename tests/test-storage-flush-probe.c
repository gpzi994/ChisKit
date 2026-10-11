#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../app/storage_flush_probe/probe.h"

static int available_rc, open_rc, flush_rc, call_count, observed;
static char calls[8];

static void record(char c) {
    assert(call_count < (int)sizeof(calls) - 1);
    calls[call_count++] = c;
}
static int fake_available(void) { record('A'); return available_rc; }
static int fake_open(const char *path, uint32_t size) {
    record('O');
    assert(strcmp(path, "/Saves/chisburner/common/storage-smoke.41.bin") == 0);
    assert(size == 1048613u);
    return open_rc;
}
static int fake_flush(void) { record('F'); return flush_rc; }
static void observer(const struct flush_probe_result *r) {
    assert(strcmp(calls, "AO") == 0);
    assert(r->open_rc == open_rc);
    assert(!r->flush_attempted);
    ++observed;
}

static void check(int available, int open, int flush, const char *expected) {
    const struct flush_probe_api api = { fake_available, fake_open, fake_flush };
    available_rc = available;
    open_rc = open;
    flush_rc = flush;
    call_count = observed = 0;
    memset(calls, 0, sizeof(calls));
    struct flush_probe_result r = flush_probe_run(&api, observer);
    assert(strcmp(calls, expected) == 0);
    assert(r.api_ok == (available == 1));
    assert(r.open_rc == (available == 1 ? open : OF_ERR_NOT_SUPPORTED));
    assert(observed == (available == 1));
    assert(r.flush_attempted == (available == 1 && open == OF_OK));
    assert(r.flush_rc == (r.flush_attempted ? flush : OF_ERR_INVALID_STATE));
}

int main(void) {
    check(0, OF_OK, OF_OK, "A");
    check(1, OF_ERR_TIMEOUT, OF_OK, "AO");
    check(1, OF_ERR_FAILED, OF_OK, "AO");
    check(1, OF_OK, OF_OK, "AOF");
    check(1, OF_OK, OF_ERR_TIMEOUT, "AOF");
    check(1, OF_OK, OF_ERR_IO, "AOF");
    puts("Sequence mock: PASS (6 cases; no writes/retries; NOT host emulation)");
    return 0;
}
