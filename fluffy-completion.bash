_fluffy_complete() {
    local local cur prev opts
    COMREPLY=()
    cur="${COMP_WORDS[COMP_CWORD]}"
    prev="${COMP_WORDS[COMP_CWORD-1]}"

    if [[ "$prev" == "fluffy" ]]; then
        local comms="play show add"
        COMPREPLY=( $(compgen -W "${comms}" -- "${cur}") )
    fi

    if [[ "$prev" == "play" ]]; then
        local games=$(fluffy only-games)
        COMPREPLY=( $(compgen -W "${games}" -- "${cur}") )
    fi
}
complete -F _fluffy_complete fluffy
