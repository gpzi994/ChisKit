# ChisKit 0.01b splash209b startup test

## 209c artwork-only follow-up

The startup-only asset `assets/branding/boot-source.png` removes the two subtitle
lines below ChisKit and the text inside the cartridge label. It was edited with
the built-in image tool, requesting only those removals while preserving the
composition. The GitHub/menu banner remains unchanged. The generated bitmap
remains 304x102 and 3876 bytes. Its rebuilt firmware.mif is byte-identical to
209b, so the tested FPGA image is reused with an explicit hash pairing gate.

This is an independent test, not a published release.

The original splash209 package incorrectly paired the rebuilt OS with old FPGA
BRAM trap targets. This package updates both os.bin and chiskit.rbf_r from the
same OS build. The application, cartridge RTL and routed circuit are unchanged.
The existing fitted database is copied, its memory initialization updated, and
timing analysis and assembly rerun. This is not a new place-and-route run.

Quartus MIF update and assembly completed with zero errors and zero warnings.
All reported constrained timing checks are nonnegative across four corners.
The unconstrained-path report is identical to the baseline: this is not full
I/O timing signoff. The pre-existing unmatched analog_fb_stride_reg constraint
warning remains. No timing exceptions were added to suppress failures.

Do not deploy only os.bin. Both files are required even when the OS ABI signature
matches: linked interrupt and syscall addresses are embedded in FPGA BRAM.

## Hardware test

1. Safely eject USB storage, exit the core, then start ChisKit again.
2. Confirm the splash and Loading screen proceed to the main UI without hanging.
3. Open settings and return; open and cancel a file selection.
4. Analyze a cartridge and confirm normal UI response. No cartridge writing is
   required for this startup test.
5. Exit and launch again, then test once after a full Pocket power cycle.

If it fails, report the last visible screen. No automatic persistent boot log is
guaranteed. Do not treat successful file copy or timing analysis as a boot pass.

The prior USB files are preserved in the package's pocket-backup directory.
Those are the failing splash209 pair, not the stable release. For a full rollback
use both files from the unchanged published 0.01b firmware instead.
