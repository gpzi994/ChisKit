#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
export OPENFPGA_SDK="${OPENFPGA_SDK:-$PWD/dependencies/openfpgaSDK}"
# The generated bitmap header is checked in. Regenerating the PNG input requires
# Pillow: python3 tests/generate-boot-splash.py
bash tests/build-gb-rtc189.sh release001b
SPLASH_BLD=bld/chiskit-splash209c bash tests/build-boot-splash.sh
cp .integration/openfpgaos/src/firmware/os/bld/chiskit-splash209c/firmware.mif quartus/firmware.mif
printf '%s\n' 'OS: .integration/openfpgaos/src/firmware/os/bld/chiskit-splash209c/os.bin' \
 'Rebuild quartus/ap_core.qpf with the matching firmware.mif before deploying.' \
 'Do not mix the new OS with a bitstream containing older trap/IRQ addresses.'
