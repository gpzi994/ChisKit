#include <stdio.h>
#include <unistd.h>
#include "of_output.h"
#include "probe.h"

#ifdef TRACE_SMOKE44
#define PROBE_LABEL "TF FLUSH smoke.44"
#else
#define PROBE_LABEL "TF FLUSH smoke.43"
#endif

#ifndef OF_PC
static void opened(const struct flush_probe_result *r) {
    printf("  OPEN: %s RC=%d\n", r->open_rc == OF_OK ? "PASS" : "FAIL",
           r->open_rc);
    printf("  WRITE COMMANDS: 0\n");
    if (r->open_rc == OF_OK)
        printf("  Stage: FLUSH WITHOUT WRITE\n");
    fflush(stdout);
}
#endif

int main(void) {
    const struct flush_probe_api api = {
        of_output_available, of_output_open, of_output_flush
    };
#ifdef OF_PC
    struct flush_probe_result r = flush_probe_run(&api, NULL);
    int pass = !r.api_ok && r.open_rc == OF_ERR_NOT_SUPPORTED &&
               !r.flush_attempted;
    printf("PC fallback: %s (hardware NOT tested)\n", pass ? "PASS" : "FAIL");
    return pass ? 0 : 1;
#else
    printf("\033[2J\033[H");
    printf("  ChisBurner\n");
    printf("  " PROBE_LABEL "\n\n");
    printf("  KEEP CARTRIDGE SLOT EMPTY\n");
    printf("  OPEN -> FLUSH; NO WRITE\n");
    printf("  REQUEST SIZE: %lu\n\n", (unsigned long)FLUSH_PROBE_SIZE);
    printf("  Stage: OPEN\n");
    fflush(stdout);

    struct flush_probe_result r = flush_probe_run(&api, opened);
    /* Do not clear: the OS prints the ACK/DONE timeout diagnostic here. */
    printf("\n  " PROBE_LABEL " RESULT\n");
    printf("  OUTPUT API: %s\n", r.api_ok ? "PASS" : "FAIL");
    printf("  OPEN: %s RC=%d\n", r.open_rc == OF_OK ? "PASS" : "FAIL",
           r.open_rc);
    printf("  WRITE COMMANDS: 0\n");
    if (r.flush_attempted) {
        printf("  FLUSH: %s RC=%d\n", r.flush_rc == OF_OK ? "PASS" : "FAIL",
               r.flush_rc);
    } else {
        printf("  FLUSH: SKIPPED\n");
    }
    printf("  DIAG COMPLETE\n");
    printf("  NOT A ROM DUMP TEST\n");
    fflush(stdout);
    while (1)
        usleep(100000);
#endif
}
