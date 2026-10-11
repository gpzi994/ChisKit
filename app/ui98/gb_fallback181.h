#ifndef CB_GB_FALLBACK181_H
#define CB_GB_FALLBACK181_H
#include <stdint.h>
/* GB-only fallback. Database verifies identity; RAM size lives in the ROM header. */
struct gb181_ops {
 void *ctx;
 int (*read)(void *,uint32_t,void *,unsigned);
 int (*safe)(void *);
};
int gb181_database_header(const uint8_t expected[80],const struct gb181_ops *ops);
int gb181_database_match(const uint8_t header[384]);
/* Result source: 0 unknown, 1 content, 2 writable-board default, 3 DB/header. */
uint32_t gb181_capacity(const uint8_t h[80],uint32_t content,int probe_rc,
                       int analysis_rc,int writable,int db_match,unsigned *source);
#endif
