#ifndef CB_CAPS137_H
#define CB_CAPS137_H
/* Exact accepted bundles; never weaken operation or write-authorization checks. */
static int cb_caps137_bundle(const of_cart_caps_t *c){
 if(c->cart_caps_raw==0x02313f03u&&c->gba_space_mask==17)return 1;
#ifdef CB_SRAM137
#ifdef CB_SRAM138
 if(c->cart_caps_raw==0x06713f03u&&c->gba_space_mask==19)return 1;
#endif
 if(c->cart_caps_raw==0x02713f03u&&c->gba_space_mask==19)return 1;
#endif
 return 0;
}
#endif
