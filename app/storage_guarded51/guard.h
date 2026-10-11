#ifndef GUARD51_H
#define GUARD51_H
#include <stdint.h>
#include "of_input_types.h"
#include "pattern.h"
enum gate51 { G_QUIET, G_LOCKED, G_HOLD_LR, G_RELEASE_LR, G_CHOOSE,
              G_RELEASE_MODE, G_CONFIRM, G_HOLD_START, G_DONE };
struct guard51 {
    enum gate51 state;
    unsigned mode,taken,unlocks,confirmations;
    uint32_t since,previous,deadline,keys,last_nonzero,edges,a_edges,b_edges;
    unsigned initialized,quiet;
};
#define LR51 (OF_BTN_L1|OF_BTN_R1)
static void lock51(struct guard51 *g,uint32_t now) {
    g->state=G_QUIET;g->mode=0;g->quiet=0;g->since=now;
}
/* Uses raw current buttons, not a possibly queued SDK pressed-event field.
 * No A/B-only stream can unlock. Every hold requires bounded poll gaps.
 * Sequence: quiet500 -> L+R2000 -> release500 -> A/B -> release500 -> START1000.
 * A key other than the expected exact combination cancels the hold/confirm.
 * One ten-second window after unlock; one consumable permission per boot.
 */
static int update51(struct guard51 *g,uint32_t keys,uint32_t now) {
    keys&=0xffffu;
    if(g->state==G_DONE)return 0;
    if(!g->initialized) {
        g->initialized=1;g->previous=now;g->keys=keys;
        lock51(g,now);
    }
    uint32_t gap=now-g->previous;g->previous=now;
    g->edges=keys&~g->keys;g->keys=keys;
    if(keys)g->last_nonzero=keys;
    if(g->edges&OF_BTN_A)++g->a_edges;
    if(g->edges&OF_BTN_B)++g->b_edges;
    if(gap>250u) {lock51(g,now);return 0;}
    if(g->state>=G_RELEASE_LR && (uint32_t)(now-g->deadline)>=10000u) {
        lock51(g,now);return 0;
    }
    switch(g->state) {
    case G_QUIET:
        if(keys){g->quiet=0;break;}
        if(!g->quiet){g->quiet=1;g->since=now;}
        if((uint32_t)(now-g->since)>=500u)g->state=G_LOCKED;
        break;
    case G_LOCKED:
        if(keys==LR51){g->state=G_HOLD_LR;g->since=now;}
        break;
    case G_HOLD_LR:
        if(keys!=LR51){lock51(g,now);break;}
        if((uint32_t)(now-g->since)>=2000u) {
            g->state=G_RELEASE_LR;g->quiet=0;g->deadline=now;++g->unlocks;
        }
        break;
    case G_RELEASE_LR:
    case G_RELEASE_MODE:
        if(keys){g->quiet=0;break;}
        if(!g->quiet){g->quiet=1;g->since=now;}
        if((uint32_t)(now-g->since)>=500u)
            g->state=g->state==G_RELEASE_LR?G_CHOOSE:G_CONFIRM;
        break;
    case G_CHOOSE:
        if(!keys)break;
        if(keys==OF_BTN_A || keys==OF_BTN_B) {
            g->mode=keys==OF_BTN_A?A51:B51;g->state=G_RELEASE_MODE;g->quiet=0;
        } else lock51(g,now);
        break;
    case G_CONFIRM:
        if(keys==OF_BTN_START){g->state=G_HOLD_START;g->since=now;}
        else if(keys)lock51(g,now);
        break;
    case G_HOLD_START:
        if(keys!=OF_BTN_START){lock51(g,now);break;}
        if((uint32_t)(now-g->since)>=1000u) {
            g->state=G_DONE;++g->confirmations;return 1;
        }
        break;
    case G_DONE:break;
    }
    return 0;
}
static unsigned take51(struct guard51 *g) {
    if(g->state!=G_DONE || g->confirmations!=1 || g->taken ||
       (g->mode!=A51 && g->mode!=B51))return 0;
    g->taken=1;return g->mode;
}
#endif
