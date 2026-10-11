#ifndef CB_UI98_H
#define CB_UI98_H
#include "model.h"
#include "analysis.h"
#ifdef CB_SAVE171
#include "../../core/save/selection171.h"
#endif
#define CB_W 320
#define CB_H 288
enum cb_page {CB_FUNCTIONS,CB_INFO,CB_TRANSFER};
struct cb_ui {
#ifdef CB_GB_SAVE175
 uint8_t gb_save175_header[80];
 uint32_t gb_save175_bytes;
 int gb_save175_ready;
#ifdef CB_GB_SAVE176
 unsigned gb_choice,gb_choice_draft;
 uint32_t gb_detected_bytes;
  int gb_probe_rc;
#ifdef CB_GB_EMPTY181
  unsigned gb_capacity_source;
#endif
#endif
#endif
 struct cb_cart cart;
 enum cb_page page;
 unsigned focus;
 int options, probing;
 unsigned language,language_draft,settings_focus; /* 0 Chinese, 1 English */
 unsigned theme,theme_draft; /* 0 light, 1 dark B */
#ifdef CB_SAVE171
 /* 0=automatic, 1..10=explicit type 0..9. Zero-init resets manual choice. */
 unsigned save_choice,save_choice_draft;
 struct cb_save171_selection save_selection;
#endif
 /* 0=parallel probe estimate, 1=confirmed retail32, 2=confirmed ChisFlash128. */
 unsigned save_profile;
 uint32_t save_observed,save_repeat;
 int save_rc;
 int save_detected_type;
 uint32_t save_detected_bytes;
 unsigned save_serial_candidate;
 char save_rom_hint[64];
 const char *notice;
 struct cb_analysis analysis;
 unsigned detail;
 int confirm, busy, finished, task_rc;
 unsigned transfer_action; /* Explicit operation for confirmation wording. */
 unsigned rtc_restore_choice,rtc_restore_pending;
 unsigned rtc_backup_choice,rtc_backup_cancelled;
 unsigned rtc_edit[6],rtc_field;
 uint32_t done, total, elapsed_ms, phase_ms;
 const char *task, *phase;
 char hash[33], path[96], source_title[17], failure_phase[48];
 char sha256[65];
 char io_diag[37], io_regs[37];
};
extern const uint32_t cb_palette[16];
extern const uint32_t cb_palette_dark[16];
void cb_render(uint8_t *fb,const struct cb_ui *ui);
/* Returns 1=refresh, 2=analyze, 3=backup ROM, 5=write ROM. */
int cb_key(struct cb_ui *ui,uint32_t pressed);
int cb_action_enabled(const struct cb_ui *,unsigned);
#ifdef CB_SAVE171
void cb_save171_update(struct cb_ui *);
#endif
#ifdef CB_ROM164
void cb_reset164(struct cb_ui *);
#endif
#endif
