#ifndef CB_GBA_HEADER140_H
#define CB_GBA_HEADER140_H
/* GBA-only diagnostic. No save, NOR identify, bank writes or file IO. */
struct cb_gba_header140 {
 unsigned char data[4][192];
 unsigned blocks[4];
 int rc[4],before,after;
};
extern struct cb_gba_header140 cb_gba_header140;
static inline void cb_gba_header140_read(struct cb_gba_header140 *r,
 const struct cb_probe_ops *o){
 memset(r,0,sizeof(*r));
 for(unsigned p=0;p<4;p++)r->rc[p]=-100;
 r->before=o->safe(o->ctx);
 if(!r->before)for(unsigned p=0;p<4;p++){
  unsigned char dummy[32];
  r->rc[p]=o->read(o->ctx,2,0,dummy,32);
  for(unsigned i=0;i<192&&!r->rc[p];i+=32){
   r->rc[p]=o->read(o->ctx,2,i,r->data[p]+i,32);
   if(!r->rc[p])r->blocks[p]++;
  }
  if(r->rc[p])break;
 }
 r->after=o->safe(o->ctx);
}
#endif
