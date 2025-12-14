#!/bin/sh

if [ "$1" = "get-vcs" ]; then
  latest_tag=$(git -C "$MESON_SOURCE_ROOT" tag --list --sort=-creatordate | grep -v continuous | head -n1)
  rev_count=$(git rev-list --count "${latest_tag}"..HEAD)
  echo "$latest_tag-r$rev_count"
elif [ "$1" = "set-dist" ]; then
  $MESONREWRITE --sourcedir="$MESON_PROJECT_DIST_ROOT" kwargs set project / version "$2"
else
  exit 1
fi
