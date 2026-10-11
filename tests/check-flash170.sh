#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/flash170-test
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined tests/test-flash170.c -o build/flash170-test/core
build/flash170-test/core
gcc -std=c11 -Wall -Wextra -Werror -Wno-misleading-indentation -Icore tests/test-flash170-os.c -o build/flash170-test/os
build/flash170-test/os
gcc -DCB_FLASH170=1 -DCB_FLASH166=1 -DCB_FLASH165=1 -DCB_ROM164=1 -DCB_EEPROM162=1 -DCB_EEPROM139=1 -DCB_SRAM137=1 -DCB_SRAM138=1 -std=c11 -Wall -Wextra -Werror -Iapp/ui98 -I.integration/openfpgaos/src/firmware/api tests/test-eeprom162-ui.c app/ui98/ui.c app/ui98/model.c app/ui98/font.c app/ui98/analysis.c -o build/flash170-test/ui
build/flash170-test/ui
bash tests/check-eeprom162-service.sh
bash tests/check-sram137-os.sh
bash tests/check-flash165.sh
gcc -DCB_FLASH166=1 -std=c11 -Wall -Wextra -Werror -Wno-misleading-indentation -fsanitize=address,undefined tests/test-flash165.c -o build/flash170-test/backup128
build/flash170-test/backup128
