#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/stream131
python3 tests/check-stream131-caps.py
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -Icore/nor \
 tests/test-stream131.c core/nor/stream131.c core/nor/recipe.c \
 core/nor/mori/chisburner_nor_db_data.c core/nor/mori/chisburner_nor_db_lookup.c \
 -o build/stream131/test-descriptor
build/stream131/test-descriptor
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -Icore/nor \
 tests/test-submit131.c core/nor/submit131.c -o build/stream131/test-submit
build/stream131/test-submit
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -DCART_STREAM131=1 \
 -Icore/nor -I.integration/openfpgaos/src/firmware/api \
 tests/test-nor108-os.c core/nor/recipe.c core/nor/stream131.c core/nor/submit131.c \
 core/nor/mori/chisburner_nor_db_data.c core/nor/mori/chisburner_nor_db_lookup.c \
 -o build/stream131/test-kernel
build/stream131/test-kernel
python3 - <<'PY'
from pathlib import Path
p = Path('app/ui98/nor130_transfer.h').read_text()
order = [
    p.index('TF -> PSRAM cache'),
    p.index('of_nor_begin_full'),
    p.index('PSRAM -> NOR / hardware'),
    p.index('Whole-NOR compare / hardware'),
    p.index('of_nor_finish(token)'),
]
assert order == sorted(order), order
print('PASS UI131 order: TF cache, begin/erase, hardware program, hardware compare, finish')
PY
