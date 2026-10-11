#ifndef CB_SAVE_READ142_H
#define CB_SAVE_READ142_H
struct cb_read142 {unsigned completed,differences,first_round,first_address;int rc;unsigned char block[64][32],single[64];};
static inline int cb_read142_run(struct cb_read142 *r,int (*read)(unsigned,void*,unsigned)){
 memset(r,0,sizeof(*r));r->first_round=r->first_address=~0u;
 for(unsigned p=0;p<64;p++){
  r->rc=read(0,r->block[p],32);if(r->rc)return r->rc;
  r->rc=read(4,&r->single[p],1);if(r->rc)return r->rc;
  r->completed++;
  for(unsigned i=0;i<33;i++){
   unsigned a=i==32?4:i;unsigned char v=i==32?r->single[p]:r->block[p][i];
   if(v!=r->block[0][a]){
    if(!r->differences){r->first_round=p;r->first_address=a;}
    r->differences++;
   }
  }
 }
 return 0; /* Differences are evidence, not transport errors. */
}
#endif
