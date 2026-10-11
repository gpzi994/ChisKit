#!/usr/bin/env bash
set -euo pipefail
revision=${1:-r4}
cd "$(dirname "$0")/.."
# Keep the established ROM engines and automatic save classifier in the UI.
make -C app/ui98 -B -j4 SAVE160=1 STREAM131=1 GPU_LEAN132=1 \
 GBA_IDENTIFY133=1 GB_IDENTIFY134=1 TIMERS135=1 \
 BUILD_DIR="$PWD/build/save160-$revision-app" USE_SDK_CONTAINER=0
# These strings belong to the compiled discovery and restore branches.
for message in 'Detecting save hardware...' 'Write SAV' "dev.160-$revision" 'Save stage'; do
 strings build/save160-$revision-app/app.elf | grep -F "$message" >/dev/null
done
