#!/bin/bash

declare -a mysconsargs
mysconsargs=(
	editor=1
	debug=1
	verbosebuild=1
	sharepath=/usr/share/games/d2x
	sdlmixer=1
	use_tracker=0
	ipv6=1
)

set -x
if git rev-parse --git-dir >/dev/null 2>&1; then
	printf -v mysconsargs[${#mysconsargs[@]}] 'extra_version=%s%s%s' \
		"$(git describe --tags 2>/dev/null)" \
		"$(git diff --name-status --quiet --exit-code --cached || echo '+')" \
		"$(git diff --name-status --quiet --exit-code || echo '*')"
fi

exec /usr/bin/scons "${mysconsargs[@]}" "$@"
