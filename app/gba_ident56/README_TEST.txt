ChisBurner smoke.56 - GBA cartridge identification

Purpose only: identify cartridge content and distinguish the supported
S29GL256N-compatible flashcart NOR profile from the generic ROM path. This
build performs no ROM dump, save access, TF write, program, or erase.

Safety boundary:
- Header reads are 24 fixed aligned ROM reads (two passes of 0x180 bytes).
- Pressing A permits one fixed FPGA-owned sequence:
  F0 reset, baseline read, AA/55/90 autoselect, ID read, F0 reset,
  98 CFI query, QRY/size reads, F0 reset.
- The application cannot provide a write address or write data. Program and
  erase opcodes do not exist in the sequence. writes_enabled remains zero.
- Do not remove/hot-swap the cartridge or power off during the brief probe.

Test the two cartridges separately. Exit the core before changing cartridges,
then start the core fresh for the next cartridge because the probe is one-shot.

1. ChisFlash containing the [1MSRAM][FTC] Chinese Pokemon Emerald image:
   keep the cart inserted, start smoke.56, photograph page 1, press A once,
   then photograph pages 3 and 4. Expected hardware result is supported NOR,
   S29GL256N compatible, physical 32 MiB, final dump plan 32 MiB. Its header
   is expected to match canonical BPEE database size 16 MiB / CRC32 1F1C08FB;
   the hardware result deliberately overrides that content length for dumping.
2. Genuine Wario Land Advance (AWAC):
   exit the core, change cartridge, start smoke.56 fresh, photograph page 1,
   press A once, then photograph pages 3 and 4. Expected content database ROM
   size is 8 MiB / CRC32 CF7585F1; hardware result is generic ROM path / no
   supported NOR profile; final dump plan is 8 MiB.

"Generic ROM path" is not an authenticity certificate. It only means this
bounded probe did not find the one supported NOR profile.
