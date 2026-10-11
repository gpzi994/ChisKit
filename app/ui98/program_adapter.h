// Instantiated for the two image lengths accepted in smoke.82 and smoke.83.
// Keep the original programming loops, final digest, end sentinel and cleanup.
#define PROGRAM65_BUFFER_BYTES 512u
#include "../gba_program65/program65.h"
#include "transfer.h"
extern const struct program65_ops cb_program_ops;
int CB_PROGRAM_ENTRY(struct cb_ui *u,int (*confirm)(void)){
 struct program65_source source=program65_scan_source(&cb_program_ops);
 if(source.rc)return source.rc;
 snprintf(u->source_title,sizeof(u->source_title),"%s",source.title);
 md5_hex53(source.md5,u->hash);
 if(!confirm())return OF_ERR_DENIED;
 struct program65_result r=program65_run(&cb_program_ops,&source,1);
 if(!r.rc)md5_hex53(r.cart_md5,u->hash);
 return r.rc;
}
