#!/usr/bin/env bash
#
# Opens a .zg document in ZeGrapher and screenshots its window.
# Don't type or click elsewhere while it runs: Spectacle shoots whatever window
# is active when the delay runs out.
#
# Usage: take-screenshot.sh [document.zg] [output.png]

here=$(dirname -- "$(realpath -- "$0")")
document=${1:-$here/vitrine.zg}
output=${2:-$here/vitrine.png}

"$here/../build-zg-debug-clang/src/ZeGrapher" "$document" &> /dev/null &
app_pid=$!

spectacle --activewindow --background --nonotify --delay 2000 --output "$output"

# SIGKILL on purpose: a graceful close makes ZeGrapher overwrite
# last-workbook.zg with the document we just opened.
kill -9 $app_pid
