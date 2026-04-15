#!/usr/bin/env bash
set -uo pipefail
source "$(dirname "$0")/helpers.sh"

# ls finds a previously added command
setup
"$RECMD" add -c "ls -la" -d /tmp
out=$("$RECMD" ls); code=$?
assert_exit "$code" 0 "ls exits 0"
assert_contains "$out" "ls -la" "ls finds added command"
teardown

# ls on empty db prints "no results"
setup
out=$("$RECMD" ls)
assert_eq "$out" "no results" "ls on empty db prints 'no results'"
teardown

# ls returns all commands
setup
"$RECMD" add -c "ls -la" -d /tmp
"$RECMD" add -c "git status" -d /home
"$RECMD" add -c "git log" -d /home
out=$("$RECMD" ls)
assert_contains "$out" "git status" "ls returns git status"
assert_contains "$out" "git log" "ls returns git log"
assert_contains "$out" "ls -la" "ls returns all commands"
teardown

# unknown option exits 1
setup
"$RECMD" ls --unknown >/dev/null 2>&1; code=$?
assert_exit "$code" 1 "ls with unknown option exits 1"
teardown

summary
