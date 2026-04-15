#!/usr/bin/env bash
set -uo pipefail
source "$(dirname "$0")/helpers.sh"

# successful add exits 0
setup
"$RECMD" add -c "ls -la" -d /tmp; code=$?
assert_exit "$code" 0 "add exits 0"
teardown

# add with explicit exit code exits 0
setup
"$RECMD" add -c "git status" -d /home -e 1; code=$?
assert_exit "$code" 0 "add with -e exits 0"
teardown

# missing -c fails with exit 1
setup
"$RECMD" add -d /tmp >/dev/null 2>&1; code=$?
assert_exit "$code" 1 "add without -c exits 1"
teardown

# missing -d fails with exit 1
setup
"$RECMD" add -c "ls" >/dev/null 2>&1; code=$?
assert_exit "$code" 1 "add without -d exits 1"
teardown

# duplicate command is stored only once
setup
"$RECMD" add -c "ls -la" -d /tmp
"$RECMD" add -c "ls -la" -d /tmp
count=$("$RECMD" search | grep -cF "ls -la")
assert_eq "$count" "1" "duplicate command appears only once in search results"
teardown

summary
