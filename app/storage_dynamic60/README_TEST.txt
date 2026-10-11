ChisBurner smoke.60 - 0192 existing/missing control

Purpose
-------
This package separates path/slot validity from dynamic creation and records
the exact address and both values for every repeated 0192 parameter read.
It reuses the smoke.59 FPGA and OS.  It never accesses the cartridge and never
issues an 0184 data write or 0188 flush.

Install
-------
Merge SD_ROOT into the root of the Pocket microSD card.  Do not rename files.
The package intentionally contains dyn60-existing.bin (8192 bytes) and does
not contain dyn60-missing.bin.

Test sequence
-------------
1. Start openFPGA -> ChisBurner -> TF Open Control 60.
2. Press B once.  This opens the packaged existing file using 0192 with
   create+resize flags.  Photograph all four result pages.
3. Exit the core and perform a full cold start before the second run.
4. Start the same instance and press A once.  This asks 0192 to create the
   absent file using the same flags and size.  Photograph all four pages.

Interpretation
--------------
- B result 0 proves the slot, supported-platform path, and ordinary open work.
- A result 1 proves create works.
- B result 0 plus A result 3 isolates failure to create-flag handling.
- B result 3 means the problem is earlier: path/slot decoding or a conflicting
  repeated parameter read.
- Page 3 shows each duplicate read offset, its first value, and repeated value.
  DIFF identifies the exact unstable word.

The 0192 resize flag may re-truncate the disposable packaged control file to
8192 bytes.  No user game/save file is named or touched by this diagnostic.
