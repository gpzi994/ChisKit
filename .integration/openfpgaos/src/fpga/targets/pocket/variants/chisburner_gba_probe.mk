# ChisBurner isolated GBA ROM-header field diagnostic.
#
# This reuses the proven ChisBurner CPU profile but opens only the fixed GBA ROM
# header window in the cartridge sequencer.  It is not the production profile.
VEXII_OUT := $(VEXII_DIR)/VexiiRiscv_chisburner.v
DEFS := INCLUDE_CARTRIDGE INCLUDE_CLK90 CHISBURNER_GBA_HEADER_PROBE
