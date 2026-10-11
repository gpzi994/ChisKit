/* Diagnostic-only fixed-layout replacement of svc_output_open in the exact
 * smoke47/49 OS. Linked at 0x1032f180; all other code/BSS/boot addresses stay.
 * Deliberately NO target command, scratch ownership or datatable size store.
 * The binding string is an authorization token, not a host path. Slot4 must
 * be bound by data.json/instance to the disposable static50.bin fixture.
 * Query is safe on the old OS too: its NULL-path check returns -3.
 */
#include <stdint.h>
#include "of_error.h"
#define STATIC50_MAGIC 0x53543530
extern int of_file_async_busy(void);
extern long of_file_size(uint32_t slot);
extern volatile int output_is_open;
extern volatile uint32_t output_open_size, output_open_slot;

__attribute__((section(".text.entry")))
int static50_service(const char *name, uint32_t size) {
    static const char expected[]="static50.bin";
    if (!name) return size==0 ? STATIC50_MAGIC : OF_ERR_INVALID_PARAM;
    if (size!=512) return OF_ERR_INVALID_PARAM;
    for (unsigned i=0;i<sizeof(expected);++i)
        if (name[i]!=expected[i]) return OF_ERR_INVALID_PARAM;
    int busy=of_file_async_busy();
    if (busy) return busy<0 ? busy : OF_ERR_BUSY;
    if (of_file_size(4)!=512) return OF_ERR_BAD_RANGE;
    output_open_slot=4;
    output_open_size=512;
    output_is_open=1;
    return 0;
}
