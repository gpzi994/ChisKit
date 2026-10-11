ChisBurner - smoke.63: identify-first dynamic 32 MiB dump

Purpose
This package combines the hardware-proven smoke.57 cartridge path with the
smoke.62 direct 0x0192 create result.  It does not ship a 32 MiB zero file.
The output is created only after cartridge identification has passed.

Exact supported cartridge for this package
[1MSRAM][FTC]口袋妖怪 - 宝石_中文典藏版[漫游&TGB联合汉化].gba
33554432 bytes
MD5 872A1A05E39006F3BEA69EFAA641B3A4

Safety boundary
1. The app first reads the header twice, checks the BPEE database entry, then
   performs the fixed one-shot NOR ID/CFI probe used by smoke.57.
2. It proceeds only for BPEE plus compatible S29GL256N 32 MiB physical NOR.
3. Before that succeeds it does not open, create, resize, or write a TF file.
4. Cartridge ROM is read-only. Save and RTC spaces are never accessed. No
   cartridge program/erase API or opcode is exposed by this build.
5. APF command 0x0188 FLUSH is never called. smoke.62 proved that normal exit
   plus a cold boot preserves data written through this path.

Install
Merge all contents of SD_ROOT into the TF root and replace same-name files.
Copy the whole package, not only the ELF, OS, or RBF.
The package intentionally contains no dump57-32m.gba output file.

Before the first run
If this path already exists from an earlier smoke.63 test, move or delete it on
the PC so the first run tests creation:
  /Saves/chisburner/common/dump57-32m.gba
Do not delete any other file.

First run
1. Fully power off. Insert the current BPEE ChisFlash. Power on and enter:
   main menu -> openFPGA -> ChisBurner -> Play Cartridge.
2. Confirm the screen says GBA DYNAMIC DUMP smoke.63 and then:
   CONTENT BPEE DB 16 MiB
   HARDWARE S29GL256N 32 MiB
   DUMP PLAN 32 MiB
   OPEN HOST 1  TF SIZE 33554432
   TF STATE: NEW FILE - SCAN SKIPPED
3. This means identification finished first and the absent file was then
   created directly at its final size. No 32 MiB zero-file boot scan occurs.
4. Arm the one-shot dump: release all 0.5 s; hold L+R 2 s; release all 0.5 s;
   press A; release all 0.5 s; hold START (+) 1 s.
5. Keep power on and do not remove the cartridge or TF card. Cartridge reads
   remain 32 bytes each. TF transfers remain 32 KiB per block.

Expected completion
Result page 1 must show PASS RC 0, 33554432/33554432 bytes, 1024/1024 blocks,
5/5 samples, SAFE RC 0, STOP OFFSET NONE, and matching cartridge/TF MD5:
872A1A05E39006F3BEA69EFAA641B3A4
Use left/right for the timing page and photograph both pages.

Cold-boot persistence check
Quit normally, fully power off, then power on and enter smoke.63 again. It will
identify the cartridge first, open the existing output (HOST 0), and read it
without writing. Expected:
  TF STATE: ROM VERIFIED
  NO REWRITE; DYNAMIC PERSISTENCE PASS
Do not arm another dump. Photograph this screen.

PC read-only verification
Run Compare-ROM63.ps1 with -DumpPath pointing to the TF file. It checks exact
length, MD5, and SHA256 against the source without modifying either file.

Failure handling
On IDENTIFICATION STOP, DYNAMIC OPEN STOP, INVALID, any nonzero RC, unexpected
size, STOP OFFSET, or hash mismatch: stop, photograph the full screen, and do
not retry in a loop. A partial/invalid dynamic file must be moved or deleted on
the PC before a deliberate retry.
