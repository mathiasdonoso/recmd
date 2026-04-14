#!/usr/bin/env bash
# Shared test helpers — source this file at the top of each test script.

RECMD="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/recmd"

PASS=0
FAIL=0

setup() {
    RECMD_DB_PATH="$(mktemp /tmp/recmd_test_XXXXXX.db)"
    export RECMD_DB_PATH
}

teardown() {
    rm -f "$RECMD_DB_PATH"
}

assert_exit() {
    local got="$1" expected="$2" msg="$3"
    if [ "$got" -eq "$expected" ]; then
        PASS=$((PASS + 1))
    else
        FAIL=$((FAIL + 1))
        echo "FAIL: $msg (exit code: got $got, expected $expected)"
    fi
}

assert_contains() {
    local output="$1" needle="$2" msg="$3"
    if echo "$output" | grep -qF "$needle"; then
        PASS=$((PASS + 1))
    else
        FAIL=$((FAIL + 1))
        echo "FAIL: $msg (expected to find: '$needle')"
        echo "      actual output: '$output'"
    fi
}

assert_not_contains() {
    local output="$1" needle="$2" msg="$3"
    if ! echo "$output" | grep -qF "$needle"; then
        PASS=$((PASS + 1))
    else
        FAIL=$((FAIL + 1))
        echo "FAIL: $msg (expected NOT to find: '$needle')"
        echo "      actual output: '$output'"
    fi
}

assert_eq() {
    local got="$1" expected="$2" msg="$3"
    if [ "$got" = "$expected" ]; then
        PASS=$((PASS + 1))
    else
        FAIL=$((FAIL + 1))
        echo "FAIL: $msg"
        echo "      expected: '$expected'"
        echo "      got:      '$got'"
    fi
}

summary() {
    local file
    file="$(basename "$0")"
    echo "$file: $PASS passed, $FAIL failed"
    [ "$FAIL" -eq 0 ]
}
