# ChisBurner dedicated hardware profile.
#
# This profile advertises the cartridge service integration and includes the
# GBA ROM read sequencer behind disabled production capability, lifecycle, and
# timing gates.  GB/GBC reads and every cartridge write path remain absent.
DEFS := INCLUDE_CARTRIDGE INCLUDE_CLK90
