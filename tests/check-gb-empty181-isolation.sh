#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
for t in check-save171-selection check-save171-service check-eeprom162-ui check-flash170 check-rtc172 check-lang174; do
 bash "tests/$t.sh"
done
bash tests/check-rom164.sh "${1:?expanded 32 MiB Emerald fixture}"
