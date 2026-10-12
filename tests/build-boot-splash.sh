#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
# Keep all release001b feature flags; only the boot display switch is new.
# Deployment MUST pair os.bin with FPGA BRAM generated from this firmware.mif.
# Matching OS ABI signatures do not validate absolute trap/IRQ jump targets.
flags=$(sed -n "s/^ EXTRA_CFLAGS='\(.*\)' .*/\1/p" tests/build-gb-rtc189.sh)
test -n "$flags"
cd .integration/openfpgaos/src/firmware/os
bld="${SPLASH_BLD:-bld/chiskit-splash209}"
make -j4 TARGET=pocket BLD="$bld" \
 EXTRA_CFLAGS="$flags -DCHISKIT_SPLASH=1" \
 "$bld/os.bin" "$bld/firmware.mif"
