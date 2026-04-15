#!/usr/bin/env bash
set -uo pipefail
source "$(dirname "$0")/helpers.sh"

# search finds a previously added command
setup
"$RECMD" add -c "ls -la" -d /tmp
out=$("$RECMD" search); code=$?
assert_exit "$code" 0 "search exits 0"
assert_contains "$out" "ls -la" "search finds added command"
teardown

# search on empty db prints "no results"
setup
out=$("$RECMD" search)
assert_eq "$out" "no results" "search on empty db prints 'no results'"
teardown

# search returns all commands
setup
"$RECMD" add -c "ls -la" -d /tmp
"$RECMD" add -c "git status" -d /home
"$RECMD" add -c "git log" -d /home
out=$("$RECMD" search)
assert_contains "$out" "git status" "search returns git status"
assert_contains "$out" "git log" "search returns git log"
assert_contains "$out" "ls -la" "search returns all commands"
teardown

# unknown option exits 1
setup
"$RECMD" search --unknown >/dev/null 2>&1; code=$?
assert_exit "$code" 1 "search with unknown option exits 1"
teardown

summary
