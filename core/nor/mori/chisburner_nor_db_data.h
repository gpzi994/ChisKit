// SPDX-License-Identifier: AGPL-3.0-only
// Adapted from MoriBunrnner-for-ESP32-S3; see PROVENANCE.md.
#ifndef CHISBURNER_NOR_DB_DATA_H
#define CHISBURNER_NOR_DB_DATA_H

#include <stddef.h>

#include "chisburner_nor_db.h"

extern const chisburner_nor_entry_t g_chisburner_nor_db_gba[];
extern const size_t g_chisburner_nor_db_gba_count;

extern const chisburner_nor_entry_t g_chisburner_nor_db_mbc5[];
extern const size_t g_chisburner_nor_db_mbc5_count;

#endif
