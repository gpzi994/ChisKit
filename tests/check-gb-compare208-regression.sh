#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/gb-compare208-tests
for t in check-gb-compare207 check-rom-stream131 check-stream131-host check-gb-save175-path check-gb-restore180-path check-gb-rtc191-clock check-gb-rtc189 check-save171-service check-eeprom162-service check-rtc172; do
 bash "tests/$t.sh" > "build/gb-compare208-tests/$t.log" 2>&1 || { tail -40 "build/gb-compare208-tests/$t.log"; exit 1; }
 tail -3 "build/gb-compare208-tests/$t.log"
done
