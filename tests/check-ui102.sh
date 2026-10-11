#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
bash tests/check-ui98-host.sh "${1:-/mnt/c/Users/lin13/Desktop/game/Pokemon - Emerald Version (USA, Europe).gba}"
make -C app/ui98 -j4
