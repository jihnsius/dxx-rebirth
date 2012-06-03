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

exec /usr/bin/scons "${mysconsargs[@]}" "$@"
