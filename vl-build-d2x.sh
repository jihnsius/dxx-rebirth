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

while read l; do
	n="${l%%=*}"
	v="${l##*=\"}"
	printf -v "$n" '%s %s' "${v%\"}" "${!n}"
done < <( grep -e '^\(C\|CXX\|LD\)FLAGS=".*"$' /etc/portage/make.conf )
export CFLAGS CXXFLAGS LDFLAGS

if git rev-parse --git-dir >/dev/null 2>&1; then
	printf -v mysconsargs[${#mysconsargs[@]}] 'extra_version=%s%s%s' \
		"$(git describe --tags 2>/dev/null)" \
		"$(git diff --name-status --quiet --exit-code --cached || echo '+')" \
		"$(git diff --name-status --quiet --exit-code || echo '*')"
	printf -v mysconsargs[${#mysconsargs[@]}] 'extra_version_build_time=%u' \
		"$(git ls-files -z | xargs -0 -r stat -c '%Y' | sort -nr | head -n1)"
fi

exec /usr/bin/scons "${mysconsargs[@]}" "$@"
