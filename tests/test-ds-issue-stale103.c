/* Reproduce stale acceptance using the unmodified production helper.
 * This is a diagnostic reproduction, NOT a claim that hardware is fixed. */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#define DS_STATUS_ACK 1u
#define DS_STATUS_DONE 2u
#define DS_STATUS_READY 32u
#define DS_ISSUE_PROBE_US 50u
static volatile uint32_t DS_STATUS, DS_COMMAND;
static uint64_t cycles;
static void fence(void) {}
static uint64_t read_cycles(void) { return ++cycles; }
static uint64_t us_to_cycles(uint64_t us) { return us; }
#include "../build/ds-issue-production103.inc"
int main(void) {
    /* Previous OPENFILE succeeded; hardware ignores the new command write. */
    DS_STATUS = DS_STATUS_ACK | DS_STATUS_DONE | DS_STATUS_READY | (1u << 2);
    DS_COMMAND = 0;
    cycles = 0;
    int rc = ds_issue_command(3u);
    assert(DS_COMMAND == 3u);
    assert(DS_STATUS == 39u);
    assert(rc == 0 && cycles == 1);
    puts("REPRODUCED: stale ACK yields success without fresh acceptance");
    return 0;
}
