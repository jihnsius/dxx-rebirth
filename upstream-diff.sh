#!/bin/sh

if [[ -n "$1" ]]; then
	tag="$1"; shift
else
	tag="$(git describe --tags --abbrev=0)"
fi

printf -v extra_version '%s%s%s' \
	"$(git describe --tags 2>/dev/null)" \
	"$(git diff --name-status --quiet --exit-code --cached || echo '+')" \
	"$(git diff --name-status --quiet --exit-code || echo '*')"
git diff --no-renames "$tag.." | sed -e '/^\+extra_version =/s|'"''|'$extra_version'|"
