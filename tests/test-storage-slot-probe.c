#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../app/storage_slot_probe/probe.h"
static char calls[16];
static int n, avail, open_rc, flush_rc, size_calls;
static long os_size, pre, post;
static void record(char c) { assert(n < 15); calls[n++] = c; }
static int available(void) { record('A'); return avail; }
static long size(const char *path) {
    if (strcmp(path, "slot:1") == 0) { record('C'); return os_size; }
    assert(strcmp(path, "slot:4") == 0);
    record('S');
    return size_calls++ == 0 ? pre : post;
}
static int open_file(const char *path, uint32_t sz) {
    record('O');
    assert(strcmp(path, FLUSH_PROBE_PATH) == 0 && sz == 1048613u);
    return open_rc;
}
static int flush_file(void) { record('F'); return flush_rc; }
static void check(int a, long os, int o, long p, long q, int f, const char *sequence) {
    struct slot_probe_api api = { available, size, open_file, flush_file };
    memset(calls, 0, sizeof(calls)); n = size_calls = 0;
    avail = a; os_size = os; open_rc = o; pre = p; post = q; flush_rc = f;
    struct slot_probe_result r = slot_probe_run(&api, NULL, NULL);
    assert(strcmp(calls, sequence) == 0);
    assert(r.open_attempted == (a == 1 && os == 137480));
    assert(r.flush_attempted == (r.open_attempted && o == OF_OK));
    if (r.open_attempted) { assert(r.slot_pre == p && r.open_rc == o); }
    if (r.flush_attempted) { assert(r.slot_post == q && r.flush_rc == f); }
}
int main(void) {
    check(0, 137480, 0, -1, -1, 0, "A");
    check(1, -1, 0, -1, -1, 0, "AC");
    check(1, 123, 0, -1, -1, 0, "AC");
    check(1, 137480, -12, -1, -1, 0, "ACSO");
    check(1, 137480, 0, -1, -1, -12, "ACSOSF");
    check(1, 137480, 0, -1, 1048613, -12, "ACSOSF");
    check(1, 137480, 0, 64, 1048613, 0, "ACSOSF");
    check(1, 137480, 0, 64, 1048613, -12, "ACSOSF");
    puts("Slot sequence mock: 8 PASS; no output writes or post-FLUSH commands");
    return 0;
}
