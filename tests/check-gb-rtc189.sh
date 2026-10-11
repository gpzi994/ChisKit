#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/gb-rtc189-tests
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -g tests/test-gb-rtc189.c core/gb_save175/gb_save175.c core/gb_save175/rtc186.c core/gb_save175/rtc189.c -o build/gb-rtc189-tests/core
build/gb-rtc189-tests/core
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -g -I.integration/openfpgaos/src/firmware/api tests/test-gb-rtc189-app.c core/gb_save175/gb_save175.c core/gb_save175/mbc7179.c core/gb_save175/rtc186.c core/gb_save175/rtc189.c -o build/gb-rtc189-tests/app
build/gb-rtc189-tests/app
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -g -Icore -I.integration/openfpgaos/src/firmware/api tests/test-gb-rtc189-os.c core/gb_save175/gb_save175.c core/gb_save175/mbc7179.c core/gb_save175/rtc186.c core/gb_save175/rtc189.c -o build/gb-rtc189-tests/os
build/gb-rtc189-tests/os
