#!/usr/bin/env bash
set -uo pipefail
source "$(dirname "$0")/helpers.sh"

# search finds a previously added command
setup
"$RECMD" add -c "ls -la" -d /tmp
out=$("$RECMD" search -c "ls"); code=$?
assert_exit "$code" 0 "search exits 0"
assert_contains "$out" "ls -la" "search finds added command"
teardown

# search with no match prints "no results"
setup
"$RECMD" add -c "git status" -d /home
out=$("$RECMD" search -c "zzznomatch")
assert_eq "$out" "no results" "search with no match prints 'no results'"
teardown

# search returns only matching commands
setup
"$RECMD" add -c "ls -la" -d /tmp
"$RECMD" add -c "git status" -d /home
"$RECMD" add -c "git log" -d /home
out=$("$RECMD" search -c "git")
assert_contains "$out" "git status" "search matches git status"
assert_contains "$out" "git log" "search matches git log"
assert_not_contains "$out" "ls -la" "search excludes non-matching command"
teardown

# missing -c fails with exit 1
setup
"$RECMD" search >/dev/null 2>&1; code=$?
assert_exit "$code" 1 "search without -c exits 1"
teardown

summary
