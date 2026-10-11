#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/nor108
verilator --cc --exe --build -j 4 --top-module chisburner_nor_bus108 \
 --Mdir build/nor108/bus core/rtl/chisburner_nor_bus108.v \
 "$PWD/tests/test-nor108-bus.cpp" >build/nor108/bus-build.log 2>&1
build/nor108/bus/Vchisburner_nor_bus108
