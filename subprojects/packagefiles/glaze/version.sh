#!/bin/sh

set -e

if [[ -z "$MESON_PROJECT_SOURCE_ROOT" ]]; then
  MESON_PROJECT_SOURCE_ROOT=$(readlink -f $(dirname "$BASH_SOURCE"))
fi

if [ "$1" = "get-vcs" ]; then
  # glaze tags its releases 'v7.8.1'. Meson compares 'v7.8.1' as a string, and
  # the leading letter then makes every comparison with a number wrong
  git -C "$MESON_PROJECT_SOURCE_ROOT" describe --tags --exact-match | sed 's/^v//'
elif [ "$1" = "set-dist" ]; then
  $MESONREWRITE --sourcedir="$MESON_PROJECT_DIST_ROOT" kwargs set project / version "$2"
else
  exit 1
fi
