#!/bin/bash

# outputs the source files that meson should handle
echo "ui_files = files("
for f in $(find . -type f -name "*.ui" | sort); do
    echo "  '$f',"
done
echo ")"
echo

echo "source_files = files("
for f in $(find . -type f -name "*.cpp" -and -not -name "main.cpp" | sort); do
    echo "  '$f',"
done
echo ")"
echo

echo "moc_header_files = files("
for f in $(grep -E -lir --include=*.h "(Q_OBJECT|Q_GADGET)" . | sort); do
    echo "  '$f',"
done
echo ")"
