#!/bin/sh

set -e

if [[ -z "$MESON_PROJECT_SOURCE_ROOT" ]]; then
  MESON_PROJECT_SOURCE_ROOT=$(readlink -f $(dirname "$BASH_SOURCE"))
fi

if [ "$1" = "get-vcs" ]; then
  git -C "$MESON_PROJECT_SOURCE_ROOT" describe --tags --exact-match
elif [ "$1" = "set-dist" ]; then
  $MESONREWRITE --sourcedir="$MESON_PROJECT_DIST_ROOT" kwargs set project / version "$2"
else
  exit 1
fi
