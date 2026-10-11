/* Fixed-layout replacement of the same diagnostic binding service used in
 * smoke.50/51. No host commands, buffer ownership or data-slot table writes.
 * Exact 1 MiB disposable slot4 only; all other OS addresses remain unchanged. */
#include <stdint.h>
#include "of_error.h"
#define STATIC52_MAGIC 0x53543532
#define STATIC52_SIZE 1048576u
extern int of_file_async_busy(void);
extern long of_file_size(uint32_t slot);
extern volatile int output_is_open;
extern volatile uint32_t output_open_size,output_open_slot;
__attribute__((section(".text.entry")))
int static52_service(const char *name,uint32_t size) {
    static const char expected[]="static52.bin";
    if(!name)return size==0?STATIC52_MAGIC:OF_ERR_INVALID_PARAM;
    if(size!=STATIC52_SIZE)return OF_ERR_INVALID_PARAM;
    for(unsigned i=0;i<sizeof(expected);++i)
        if(name[i]!=expected[i])return OF_ERR_INVALID_PARAM;
    int busy=of_file_async_busy();
    if(busy)return busy<0?busy:OF_ERR_BUSY;
    if(of_file_size(4)!=STATIC52_SIZE)return OF_ERR_BAD_RANGE;
    output_open_slot=4;output_open_size=STATIC52_SIZE;output_is_open=1;
    return 0;
}
