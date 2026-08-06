#!/usr/bin/env bash

set -e

if [[ -z "$MESON_SOURCE_ROOT" ]]; then
  MESON_SOURCE_ROOT=$(readlink -f $(dirname "$BASH_SOURCE"))
fi

if [ "$1" = "get-vcs" ]; then
  # skip tags that don't end in a number: probably not a version
  latest_tag=$(git -C "$MESON_SOURCE_ROOT" tag --list --sort=-creatordate | grep -v continuous | grep -E '[0-9]$' | head -n1)
  rev_count=$(git -C "$MESON_SOURCE_ROOT" rev-list --count "${latest_tag}"..HEAD)
  if [[ $latest_tag =~ ^(.*)([0-9]+)$ ]]; then
    increment=1
    [[ "$rev_count" == "0" ]] && increment=0
    digits="${BASH_REMATCH[2]}"
    echo "${BASH_REMATCH[1]}$(( digits + increment ))"
  else
    echo "Tag $latest_tag has unsupported format"
    exit 1
  fi
elif [ "$1" = "set-dist" ]; then
  $MESONREWRITE --sourcedir="$MESON_PROJECT_DIST_ROOT" kwargs set project / version "$2"
else
  exit 1
fi
