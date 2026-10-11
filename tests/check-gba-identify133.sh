#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
gcc -D_DEFAULT_SOURCE -DCB_GBA_IDENTIFY133=1 -std=c11 -Wall -Wextra -Werror \
 -Wno-misleading-indentation -Wno-error=format-truncation -fsanitize=address,undefined -Iapp/ui98 \
 -I.integration/openfpgaos/src/firmware/api tests/test-gba-identify133.c \
 app/ui98/analysis.c app/ui98/model.c -o build/test-gba-identify133
build/test-gba-identify133
