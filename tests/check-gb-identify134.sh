#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
gcc -D_DEFAULT_SOURCE -DCB_GB_IDENTIFY134=1 -std=c11 -Wall -Wextra -Werror -Wno-misleading-indentation -Wno-error=format-truncation -fsanitize=address,undefined -Iapp/ui98 -I.integration/openfpgaos/src/firmware/api tests/test-gb-identify134.c app/ui98/analysis.c app/ui98/model.c -o build/test-gb-identify134
build/test-gb-identify134
