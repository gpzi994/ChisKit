/* GB/GBC only. No GBA enums, address flags or RTC-tail format. */
#ifndef CB_GB_SAVE175_H
#define CB_GB_SAVE175_H
#include <stdint.h>
#include <stddef.h>
enum gb175_rc { GB175_OK=0, GB175_ARGUMENT=-1, GB175_UNSUPPORTED=-2,
 GB175_IDENTITY=-3, GB175_IO=-4, GB175_CHANGED=-5, GB175_CANCELLED=-6,
 GB175_CLEANUP=-7, GB175_FILE=-8 };
enum gb175_mapper { GB177_PLAIN=0, GB177_MBC1=1, GB178_MBC2=2, GB175_MBC3=3, GB175_MBC5=5, GB179_MBC7=7, GB175_MBC30=30 };
struct gb175_plan { uint32_t bytes; unsigned mapper,banks,rumble,rtc_declared; };
/* Header bytes 0x100..0x14f, not a GBA header. A GBA bus is rejected. */
int gb175_plan_header(unsigned bus,const uint8_t header[80],struct gb175_plan *);
/* Independent GB ordinary-RAM capacity planning. selected=0 means probe the
 * mapper-safe span, not a claim of physical capacity. Legacy API unchanged. */
int gb176_plan(unsigned bus,const uint8_t header[80],uint32_t selected,struct gb175_plan *);
uint32_t gb176_content_bytes(const void *data,uint32_t bytes);
int gb177_plan(unsigned bus,const uint8_t header[80],uint32_t selected,struct gb175_plan *);
uint32_t gb177_content_bytes(const void *data,uint32_t bytes);
int gb178_plan(unsigned bus,const uint8_t header[80],uint32_t selected,struct gb175_plan *);
int gb179_plan(unsigned bus,const uint8_t header[80],uint32_t selected,struct gb175_plan *);
int gb175_chunk(const struct gb175_plan *,uint32_t offset,uint32_t maximum,
 unsigned *bank,uint16_t *address,uint32_t *length);
/* Production adapter must acquire GB ownership, freshly check lifecycle and
 * enforce timeouts. read_ram accepts only A000..BFFF; control must enforce
 * 0000:{00,0A}, 4000:RAM bank. There is NO save-data write callback.
 * acquire failure must self-release; release always drops electrical ownership.
 * alive is false on removal/epoch change; cancellation must not impersonate removal.
 * All supplied buffers must be disjoint cached SDRAM on Pocket, never PSRAM.
 */
struct gb175_bus {
 void *ctx;
 int (*acquire)(void *);
 int (*alive)(void *);
 int (*cancelled)(void *);
 int (*read_header)(void *,uint8_t [80]);
 int (*control)(void *,uint16_t,uint8_t);
 int (*read_ram)(void *,uint16_t,void *,uint32_t);
 int (*release)(void *);
};
struct gb175_output {
 void *ctx;
 int (*create_new)(void *,uint32_t); /* Must not overwrite; self-clean on failure. */
 int (*write)(void *,uint32_t,const void *,uint32_t);
 int (*finish)(void *);
 int (*read)(void *,uint32_t,void *,uint32_t);
 void (*abort)(void *); /* Preserve old files; mark incomplete new output. */
};
struct gb175_report { int rc,detail,cleanup_rc;uint32_t compared,written,verified; };
int gb179_capture(const struct gb175_bus *,unsigned,const uint8_t [80],uint32_t,
 void *,size_t,void *,uint32_t,struct gb175_report *);
int gb178_capture(const struct gb175_bus *,unsigned,const uint8_t [80],uint32_t,
 void *,size_t,void *,uint32_t,struct gb175_report *);
int gb175_capture(const struct gb175_bus *,unsigned bus,const uint8_t expected[80],
 void *snapshot,size_t capacity,void *scratch,uint32_t chunk,struct gb175_report *);
int gb176_capture(const struct gb175_bus *,unsigned bus,const uint8_t expected[80],
 uint32_t selected,void *snapshot,size_t capacity,void *scratch,uint32_t chunk,struct gb175_report *);
int gb177_capture(const struct gb175_bus *,unsigned bus,const uint8_t expected[80],
 uint32_t selected,void *snapshot,size_t capacity,void *scratch,uint32_t chunk,struct gb175_report *);
int gb175_backup(const struct gb175_bus *,const struct gb175_output *,
 unsigned bus,const uint8_t expected[80],void *snapshot,size_t capacity,
 void *scratch,uint32_t chunk,struct gb175_report *);
#endif
