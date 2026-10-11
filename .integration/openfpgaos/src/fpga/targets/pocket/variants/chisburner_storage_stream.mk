# ChisBurner isolated TF-card streaming diagnostic.
#
# This profile contains no cartridge interface capability.  It adds only a
# build marker around the proven openfpgaOS CPU/data-slot path so smoke.28 can
# validate OPENFILE, block WRITE and FLUSH independently from cartridge reads.
VEXII_OUT := $(VEXII_DIR)/VexiiRiscv_chisburner.v
DEFS := INCLUDE_CLK90 CHISBURNER_STORAGE_STREAM
