# ChisBurner isolated GBA ROM 4 KiB field diagnostic.
#
# Only aligned 32-byte reads inside ROM byte addresses 0x0000-0x0FFF are
# reachable.  This is not the production profile.
VEXII_OUT := $(VEXII_DIR)/VexiiRiscv_chisburner.v
DEFS := INCLUDE_CARTRIDGE INCLUDE_CLK90 CHISBURNER_GBA_ROM4K_PROBE
