#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/gb-mbc7-restore185-test
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -g -Icore tests/test-gb-mbc7-restore185.c core/gb_save175/gb_save175.c core/gb_save175/mbc7179.c core/gb_save175/restore180.c core/gb_save175/mbc7_restore185.c -o build/gb-mbc7-restore185-test/core
build/gb-mbc7-restore185-test/core '/mnt/c/Users/lin13/Desktop/dumppocket/Korokoro Kirby (Japan).sav'
gcc -DCB_GB_RESTORE185=1 -DCB_GB_EMPTY181=1 -DCB_GB_RESTORE180=1 -DCB_GB_SAVE179=1 -DCB_GB_SAVE178=1 -DCB_GB_SAVE177=1 -DCB_GB_SAVE176=1 -DCB_GB_SAVE175=1 -DCB_ROM164=1 -DCB_LANG174=1 -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -g -I.integration/openfpgaos/src/firmware/api tests/test-gb-restore180-app.c app/ui98/ui.c app/ui98/model.c app/ui98/font.c app/ui98/analysis.c -o build/gb-mbc7-restore185-test/app
build/gb-mbc7-restore185-test/app
