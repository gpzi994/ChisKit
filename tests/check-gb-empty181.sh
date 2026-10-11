#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/gb-empty181-tests
flags='-DCB_GB_EMPTY181=1 -DCB_GB_RESTORE180=1 -DCB_GB_SAVE179=1 -DCB_GB_SAVE178=1 -DCB_GB_SAVE177=1 -DCB_GB_SAVE176=1 -DCB_GB_SAVE175=1 -DCB_ROM164=1 -DCB_LANG174=1'
gcc $flags -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -g \
 -I.integration/openfpgaos/src/firmware/api tests/test-gb-empty181.c \
 app/ui98/model.c app/ui98/gb_fallback181.c -o build/gb-empty181-tests/policy
build/gb-empty181-tests/policy "$@"
gcc $flags -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -g \
 -I.integration/openfpgaos/src/firmware/api tests/test-gb-restore180-app.c \
 app/ui98/ui.c app/ui98/model.c app/ui98/font.c app/ui98/analysis.c \
 -o build/gb-empty181-tests/restore-app
build/gb-empty181-tests/restore-app
bash tests/check-gb-restore180.sh
