# recmd

Record shell commands to a local SQLite database and search them later.

## Motivation

I use tmux with multiple sessions and kept losing track of commands I ran in different contexts. Shell history
is split per session, so searching through old commands becomes a pain. I know there are existing solutions
like [atuin](https://github.com/atuinsh/atuin), but I wanted something more personal — so I built this CLI that
stores commands in a local SQLite database, letting me search them later regardless of where they were executed.

## Build

```
make
```

## Install

```
sudo make install
```

## Bash integration

```bash
if command -v recmd &>/dev/null; then
    __recmd_preexec() {
        [[ -n "$COMP_LINE" ]] && return
        [[ "$BASH_COMMAND" == __recmd_* ]] && return
        __recmd_last_cmd="${BASH_COMMAND}"
    }

    __recmd_precmd() {
        local exit_code=$?
        if [ -n "${__recmd_last_cmd:-}" ]; then
            recmd add -c "${__recmd_last_cmd}" -d "${PWD}" -e "${exit_code}" 2>/dev/null
            __recmd_last_cmd=""
        fi
    }

    if [[ -n "${__bp_preexec_interactive_mode+x}" ]]; then
        preexec_functions+=(__recmd_preexec)
        precmd_functions+=(__recmd_precmd)
    else
        trap '__recmd_preexec' DEBUG
        PROMPT_COMMAND="__recmd_precmd${PROMPT_COMMAND:+;$PROMPT_COMMAND}"
    fi

    if command -v fzf &>/dev/null; then
        __recmd_fzf_search() {
            local selected
            selected=$(recmd search -c "${READLINE_LINE}" | fzf --height 40% --reverse)
            if [ -n "$selected" ]; then
                READLINE_LINE="$selected"
                READLINE_POINT=${#selected}
            fi
        }
        bind -x '"\C-r": __recmd_fzf_search'
    fi
fi
```

Then reload your shell:

```
source ~/.bashrc
```


