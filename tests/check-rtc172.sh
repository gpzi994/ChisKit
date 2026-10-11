#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/rtc172-test
for test in rtc172 rtc172-write rtc172-os rtc167-os rtc172-app; do
 gcc -D_POSIX_C_SOURCE=200809L -std=c11 -Wall -Wextra -Werror -fsanitize=undefined \
  -I.integration/openfpgaos/src/firmware/api -Icore \
  "tests/test-$test.c" -o "build/rtc172-test/$test"
 "build/rtc172-test/$test"
done
