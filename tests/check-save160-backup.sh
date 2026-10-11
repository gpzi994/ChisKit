#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=undefined \
 tests/test-save160-backup.c -o build/save160-backup-test
build/save160-backup-test
