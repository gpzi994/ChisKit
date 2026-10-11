ChisBurner smoke.61 - 0192 numeric-field endian test

This package does not access the cartridge and does not issue 0x0188 FLUSH.
It changes only how the two numeric fields in the 0x0192 open structure are
presented on BRIDGE: flags must be 0x00000003 and size must be 0x00002000.

The target /Saves/chisburner/common/dyn61-endian.bin is intentionally absent.
Cold boot, select TF Create Endian 61, and press A once. A creates the 8192-byte
file, writes eight 1024-byte blocks through 0x0184, then reads and compares all
8192 bytes. Send both result pages.

Expected: HOST 1 CREATED, FLAGS WIRE 00000003, SIZE WIRE 00002000,
WRITE 8/8, READ 8192/8192, MATCH 8192/8192, CRC D46FA793, RESULT PASS.

After PASS, quit normally, cold power off/on without copying the package again,
launch the same instance and press B. Expected: HOST 0 OPENED and RESULT PASS.
