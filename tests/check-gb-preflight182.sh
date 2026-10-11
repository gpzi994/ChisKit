#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/gb-preflight182-tests
gcc -DCB_GB_SAVE175=1 -DCB_GB_SAVE179=1 -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -g \
 -I.integration/openfpgaos/src/firmware/api tests/test-gb-preflight182.c app/ui98/model.c \
 -o build/gb-preflight182-tests/preflight
build/gb-preflight182-tests/preflight
bash tests/check-gb-empty181.sh "$@"
