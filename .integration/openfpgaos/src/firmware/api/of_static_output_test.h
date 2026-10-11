/* smoke.50-only SDK adapter. No host OPEN, resize or MMIO in the app.
 * The diagnostic OS repurposes the output_open service-table entry:
 * (NULL,0) is a non-mutating version query; ("static50.bin",512) binds the
 * already declared slot4 to the existing bounded WRITE/FLUSH services.
 * This is NOT the production dynamic-output API. An old OS fails the query
 * without opening a file. Never bind or write after a query mismatch.
 */
#ifndef OF_STATIC_OUTPUT_TEST_H
#define OF_STATIC_OUTPUT_TEST_H
#include "of_output.h"
#define OF_STATIC50_MAGIC 0x53543530
#define OF_STATIC50_SIZE 512u
#define OF_STATIC50_NAME "static50.bin"
static inline int of_static50_query(void) {
    return of_output_open(NULL,0);
}
static inline int of_static50_bind(void) {
    if (of_static50_query()!=OF_STATIC50_MAGIC) return OF_ERR_NOT_SUPPORTED;
    return of_output_open(OF_STATIC50_NAME,OF_STATIC50_SIZE);
}
#endif
