#!/bin/bash

echo "ui_files = ["
for f in $(find . -type f -name "*.ui"); do
    echo "    '$f',"
done
echo "]"
echo

echo "source_files = ["
for f in $(find . -type f -name "*.cpp"); do
    echo "    '$f',"
done
echo "]"
echo

echo "moc_header_files = ["
for f in $(grep -E -lir --include=*.h "Q_OBJECT" .); do
    echo "    '$f',"
done
echo "]"