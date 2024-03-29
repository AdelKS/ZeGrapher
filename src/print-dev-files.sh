#!/bin/bash

echo "ui_files = files("
for f in $(find . -type f -name "*.ui"); do
    echo "  '$f',"
done
echo ")"
echo

echo "source_files = files("
for f in $(find . -type f -name "*.cpp"); do
    echo "  '$f',"
done
echo ")"
echo

echo "moc_header_files = files("
for f in $(grep -E -lir --include=*.h "(Q_OBJECT|Q_GADGET)" .); do
    echo "  '$f',"
done
echo ")"
