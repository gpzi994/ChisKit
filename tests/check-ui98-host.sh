#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/ui98-preview
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -g \
 -Iapp/ui98 -I.integration/openfpgaos/src/firmware/api tests/test-ui98.c tests/test-ui98-analysis.c \
 app/ui98/model.c app/ui98/ui.c app/ui98/font.c app/ui98/analysis.c -o build/test-ui98
./build/test-ui98 "${1:?Supply local GBA header fixture}"
gcc -D_DEFAULT_SOURCE -std=c11 -Wall -Wextra -Werror -Wno-unused-function \
 -Wno-misleading-indentation -fsanitize=address,undefined -Iapp/ui98 \
 -I.integration/openfpgaos/src/firmware/api tests/test-ui98-transfer.c \
 app/ui98/analysis.c app/ui98/model.c app/ui98/ui.c app/ui98/font.c -o build/ui98-transfer-host
build/ui98-transfer-host
