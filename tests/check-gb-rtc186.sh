#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/gb-rtc186-tests
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -g tests/test-gb-rtc186-header.c -o build/gb-rtc186-tests/header
build/gb-rtc186-tests/header
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -g tests/test-gb-rtc186.c core/gb_save175/gb_save175.c core/gb_save175/rtc186.c -o build/gb-rtc186-tests/core
build/gb-rtc186-tests/core
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -g -Icore -I.integration/openfpgaos/src/firmware/api tests/test-gb-rtc186-os.c core/gb_save175/gb_save175.c core/gb_save175/mbc7179.c core/gb_save175/rtc186.c -o build/gb-rtc186-tests/os
build/gb-rtc186-tests/os
