#!/usr/bin/env bash

set -e
set -o pipefail

if [[ -z "$MESON_SOURCE_ROOT" ]]; then
  MESON_SOURCE_ROOT=$(readlink -f $(dirname "$BASH_SOURCE"))
fi

meson introspect "${MESON_SOURCE_ROOT}/meson.build" --projectinfo | jq --raw-output ".version"
