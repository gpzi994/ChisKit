#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/gb-mbc2-restore184-tests
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -g -Icore tests/test-gb-mbc2-restore184.c core/gb_save175/gb_save175.c core/gb_save175/mbc7179.c core/gb_save175/restore180.c -o build/gb-mbc2-restore184-tests/core
build/gb-mbc2-restore184-tests/core '/mnt/c/Users/lin13/Desktop/dumppocket/Tenchi o Kurau (Japan).sav'
bash tests/check-gb-database183.sh
bash tests/check-gb-restore180-rtl.sh
bash tests/check-gb-restore180-path.sh
bash tests/check-gb-restore180-regression.sh
