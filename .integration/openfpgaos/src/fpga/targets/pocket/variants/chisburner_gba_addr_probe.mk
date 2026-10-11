# ChisBurner isolated GBA ROM high-address field diagnostic.
#
# Only eleven fixed 32-byte ROM addresses are reachable, twice. This is not
# the production profile.
VEXII_OUT := $(VEXII_DIR)/VexiiRiscv_chisburner.v
DEFS := INCLUDE_CARTRIDGE INCLUDE_CLK90 CHISBURNER_GBA_ADDR_PROBE
