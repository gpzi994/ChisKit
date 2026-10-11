ChisBurner smoke.59 - OPENFILE parameter visibility

This package does not access the cartridge, write data blocks, or call 0x0188
FLUSH. It issues exactly one 0x0192 OPENFILE request after A is pressed, then
freezes the hardware trace. The target file is absent from the package.

Merge every directory under SD_ROOT into the TF root and approve replacement.
Cold power off/on. Select "TF Open Trace 59.json" and confirm that the screen
says "TF OPEN TRACE smoke.59". Press A once. Send photographs of all three
pages; LEFT/RIGHT changes pages.

Page 1 reports the raw Pocket status sequence. Page 2 reports every 264-byte
OPENFILE parameter word that the FPGA queued for the host, including the path,
create+resize flags, and requested size. Page 3 reports trace integrity.

Do not rerun the test without a cold power cycle because the recorder is
single-use. If the file was successfully created, delete
Saves/chisburner/common/dyn59-a7c9.bin before a fresh first-run test.
