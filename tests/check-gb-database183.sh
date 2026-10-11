#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/gb-database183-tests
gcc -DCB_GB_SAVE179=1 -DCB_GB_SAVE178=1 -DCB_GB_SAVE177=1 -DCB_GB_SAVE176=1 -DCB_GB_SAVE175=1 -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -g tests/test-gb-database183.c app/ui98/gb_fallback181.c app/ui98/model.c -o build/gb-database183-tests/route
build/gb-database183-tests/route
bash tests/check-gb-preflight182.sh '/mnt/c/Users/lin13/Desktop/dumppocket/Pocket Monsters Kin (Japan) (SGB Enhanced) (GB Compatible).gbc' '/mnt/c/Users/lin13/Desktop/dumppocket/Tetris DX (World) (SGB Enhanced) (GB Compatible).gbc'
bash tests/check-gb-empty181-isolation.sh /mnt/c/Users/lin13/Downloads/*1986*.gba
