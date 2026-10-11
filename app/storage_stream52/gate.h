#ifndef GATE52_H
#define GATE52_H
#include "../storage_guarded51/guard.h"
/* Preserve the accepted smoke51 timing/one-shot implementation. Only mode A
 * is usable here; choosing B cancels rather than authorizing any operation. */
static int gate_tick52(struct guard51 *g,uint32_t keys,uint32_t now) {
    int confirmed=update51(g,keys,now);
    if(g->mode==B51){lock51(g,now);return 0;}
    return confirmed;
}
#endif
