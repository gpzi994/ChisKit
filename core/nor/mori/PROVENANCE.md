# Mori NOR database port

Source: user-supplied `C:/ONS_FPGA/references/MoriBunrnner-for-ESP32-S3`, without Git metadata.
Imported 2026-09-09 under the source AGPL-3.0 license; see COPYING.
Original author/project attribution: MoriBunrnner-for-ESP32-S3.

All original GBA and MBC5 entries, masks, family parameters, flags and lookup functions are retained. Changes: namespace burner/BURNER to chisburner/CHISBURNER, local include path, attribution headers. Matching a record does not authorize writes.

Smoke131-r8 adds a ChisBurner compatibility entry for MX29GL640EB (C2 7E 10 00), not present in the supplied Mori table. It requires repeated ID/CFI acquisition and the exact 8 MiB bottom-boot geometry and 32-byte buffer already recognized by the legacy ChisBurner analyzer. No absent-CFI fallback is permitted for this extension. Hardware qualification remains pending.

The separate stream executor follows the block/single-byte separation in Mori's `burner_gbc_flash_lowlevel.c`, `burner_gbc_gbx_program_block` (1298 onward): respect mapper/sector boundaries, then execute and poll single writes inside a block. ChisBurner implements that loop in FPGA; the imported source is not modified. PSRAM remains read-only ROM input to the programming engine.

Original source SHA-256:

- `main/burner/db/burner_nor_db.h`: `08EB78C0021D0BCF7799EDAA0A37055516F14587422242223E0BD0F9C58EA40A`
- `main/burner/db/burner_nor_db_data.h`: `8A4F8A02B4C496D093C886D40B23B84380526CE279CF89F6660C6AFB4A987073`
- `main/burner/db/burner_nor_db_data.c`: `A9B3C8A4A34E459DE19D50661B3ADDD26D972162717648033CC56D71940EBDDF`
- `main/burner/db/burner_nor_db_lookup.c`: `CA9357CC3F33B818D734DCFB16D4B43FAD51A1D77D89B787B059DFC168FC068F`
- `LICENSE`: `8486A10C4393CEE1C25392769DDD3B2D6C242D6EC7928E1414EFFF7DFB2F07EF`
