ChisBurner - smoke.55 GBA database identification

This package performs only two stable reads of the first 0x180 ROM bytes.
It never dumps a ROM and contains no cartridge, save-memory, or TF write call.

Use the same inserted ChisFlash cartridge and open:
  openFPGA -> ChisBurner -> Play Cartridge

The first line must be "GBA DB smoke.55". If an older smoke number appears,
the replacement did not take effect; remove the stale ChisBurner core/assets and
merge the complete SD_ROOT again.

Expected for the current patched Gyakuten Saiban image:
  HEADER READ 24/24 RC 0 REPEAT PASS
  LOGO PASS FIXED96 PASS CHECKSUM PASS
  HEADER SHA1: AE78CB07CB1CA398CD34379A67CF7E01BE79F0F4
  DATABASE: NOT FOUND
  DUMP PLAN: 33554432 BYTES (32 MiB)
  POLICY: UNKNOWN => FULL 32 MiB

Page 2 must show 2923/2923 entries and RUNTIME INDEX COMPLETE PASS.
Page 3 must say NO DUMP PERFORMED IN smoke.55.

The complete pinned ChisBurner db_AGB.json is included in the package. The app
uses a generated compact index of every entry and the functional fields needed
for dump planning: header SHA-1, rs, rc, st, ss, gc, and presence/value flags
for rv and 3d. Human-readable metadata remains in the canonical JSON snapshot.
