ChisBurner smoke.62 - direct 0192 on known-good OS

smoke.61 is retired because its newly linked os.bin stopped at the blue boot
screen before the application loaded.  This package restores the exact
hardware-verified smoke.60 os.bin and confines the endian experiment to this
diagnostic application.

The target /Saves/chisburner/common/dyn62-direct.bin is intentionally absent.
The application directly drives the existing data-slot bridge only for this
test; it never accesses the cartridge and never issues 0x0188 FLUSH.

Cold boot, select TF Direct Create 62, and press A once.  Expected:
HOST 1 CREATED, FLAGS WIRE 00000003, SIZE WIRE 00002000, WRITE 8/8,
READ 8/8, MATCH 8192/8192, CRC D46FA793, RESULT PASS.

After PASS, quit normally, cold power off/on without copying the package
again, launch the same instance, and press B.  Expected: HOST 0 OPENED,
FLAGS/SIZE WIRE both zero, READ 8/8, and RESULT PASS.
