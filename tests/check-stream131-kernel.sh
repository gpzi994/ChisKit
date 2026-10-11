#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/../.integration/openfpgaos/src/firmware/os"
# Compile development objects separately; do not replace boot.rom or release OS.
make TARGET=pocket BLD=bld/stream131-kernel \
 EXTRA_CFLAGS="-DCART_NOR108=1 -DCART_NOR109=1 -DCART_NOR110=1 -DCART_STREAM131=1" \
 bld/stream131-kernel/kernel/nor108.o \
 bld/stream131-kernel/nor108/stream131.o bld/stream131-kernel/nor108/submit131.o
