#!/usr/bin/env bash
#------------------------------------------------------------------------------
# SPDX-License-Identifier: Apache-2.0
# SPDX-FileType: SOURCE
# SPDX-FileCopyrightText: (c) 2026, ChisBurner contributors
#------------------------------------------------------------------------------

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
API="$ROOT/src/firmware/api"
BUILD="$(mktemp -d)"
trap 'rm -rf "$BUILD"' EXIT

CC_BIN="${CC:-cc}"
CFLAGS=(-std=c11 -Wall -Wextra -Werror -I"$API")

"$CC_BIN" "${CFLAGS[@]}" "$ROOT/tools/test_cart_api.c" \
    -o "$BUILD/test_cart_api"
"$BUILD/test_cart_api"

"$CC_BIN" "${CFLAGS[@]}" -DOF_PC "$ROOT/tools/test_cart_api_pc.c" \
    -o "$BUILD/test_cart_api_pc"
"$BUILD/test_cart_api_pc"

printf 'cart API tests passed\n'
