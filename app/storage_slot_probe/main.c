#include <stdio.h>
#include <unistd.h>
#include "of_output.h"
#include "probe.h"

static int slot_api_available(void) {
#ifdef OF_PC
    return 0;
#else
    return of_output_available() && OF_OUTPUT_SVC_HAS(file_size);
#endif
}
static long sdk_slot_size(const char *path) {
#ifdef OF_PC
    (void)path;
    return -1;
#else
    return OF_SVC->file_size(path);
#endif
}
#ifndef OF_PC
static void before_open(const struct slot_probe_result *r) {
    printf("  OS SLOT1: %ld\n", r->os_size);
    printf("  SLOT4 PRE: %ld\n", r->slot_pre);
    printf("  Stage: OPEN\n");
    fflush(stdout);
}
static void before_flush(const struct slot_probe_result *r) {
    printf("  OPEN: PASS RC=%d\n", r->open_rc);
    printf("  SLOT4 POST: %ld\n", r->slot_post);
    printf("  EXPECT: %lu\n", (unsigned long)FLUSH_PROBE_SIZE);
    printf("  Stage: FLUSH WITHOUT WRITE\n");
    fflush(stdout);
}
#endif
int main(void) {
    const struct slot_probe_api api = {
        slot_api_available, sdk_slot_size, of_output_open, of_output_flush
    };
#ifdef OF_PC
    struct slot_probe_result r = slot_probe_run(&api, NULL, NULL);
    int pass = !r.api_ok && !r.open_attempted && !r.flush_attempted;
    printf("PC unsupported fallback: %s (not hardware emulation)\n", pass ? "PASS" : "FAIL");
    return pass ? 0 : 1;
#else
    printf("\033[2J\033[H  ChisBurner\n");
    printf("  TF SLOT smoke.45\n\n");
    printf("  KEEP CARTRIDGE SLOT EMPTY\n");
    printf("  WRITE COMMANDS: 0\n");
    printf("  Stage: SLOT METADATA\n");
    fflush(stdout);
    struct slot_probe_result r = slot_probe_run(&api, before_open, before_flush);
    /* Preserve the OS ACK/DONE timeout trace. All metadata queries were
     * completed BEFORE FLUSH, so a timeout is never followed by file I/O. */
    printf("\n  TF SLOT smoke.45 RESULT\n");
    printf("  API: %s\n", r.api_ok ? "PASS" : "FAIL");
    printf("  OS SLOT1: %ld\n", r.os_size);
    printf("  SLOT4 PRE/POST: %ld / %ld\n", r.slot_pre, r.slot_post);
    if (r.open_attempted)
        printf("  OPEN: %s RC=%d\n", r.open_rc == OF_OK ? "PASS" : "FAIL", r.open_rc);
    else
        printf("  OPEN: SKIPPED\n");
    if (r.flush_attempted)
        printf("  FLUSH: %s RC=%d\n", r.flush_rc == OF_OK ? "PASS" : "FAIL", r.flush_rc);
    else
        printf("  FLUSH: SKIPPED\n");
    printf("  -1 SIZE = UNRESOLVED\n");
    printf("  DIAG COMPLETE\n");
    fflush(stdout);
    while (1)
        usleep(100000);
#endif
}
