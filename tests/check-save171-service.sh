#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/save171-test
gcc -DCB_SAVE171=1 -std=c11 -Wall -Wextra -Werror -fsanitize=undefined \
 -Icore -Icore/nor -Icore/save -I.integration/openfpgaos/src/firmware/api \
 tests/test-eeprom162-service.c -o build/save171-test/eeprom-service
build/save171-test/eeprom-service
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -Icore \
 tests/test-save171-blank.c -o build/save171-test/blank
build/save171-test/blank
gcc -DCB_SAVE171=1 -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -Icore \
 tests/test-save171-owned.c core/save/detect139.c core/save/restore158.c -o build/save171-test/owned
build/save171-test/owned
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=undefined -Icore \
 -I.integration/openfpgaos/src/firmware/api tests/test-save171-dispatch.c \
 -o build/save171-test/dispatch
build/save171-test/dispatch
gcc -DCB_SAVE171=1 -std=c11 -Wall -Wextra -Werror -Wno-misleading-indentation \
 -fsanitize=address,undefined -Iapp/ui98 -I.integration/openfpgaos/src/firmware/api \
 tests/test-save171-app.c -o build/save171-test/app
build/save171-test/app
bash tests/check-save160-service.sh
bash tests/check-eeprom162-service.sh
