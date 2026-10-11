/* smoke.52 SDK/OS diagnostic contract. Binding is RAM-only, not host OPEN. */
#ifndef OF_STATIC_OUTPUT52_H
#define OF_STATIC_OUTPUT52_H
#include "of_output.h"
#define OF_STATIC52_MAGIC 0x53543532
#define OF_STATIC52_SIZE 1048576u
#define OF_STATIC52_NAME "static52.bin"
static inline int of_static52_query(void){return of_output_open(NULL,0);}
static inline int of_static52_bind(void) {
    if(of_static52_query()!=OF_STATIC52_MAGIC)return OF_ERR_NOT_SUPPORTED;
    return of_output_open(OF_STATIC52_NAME,OF_STATIC52_SIZE);
}
#endif
