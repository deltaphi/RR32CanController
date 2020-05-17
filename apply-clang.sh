#!/bin/sh

CLANG_BINARY="/c/Program Files/LLVM/bin/clang-format.exe"

for filename in `find src -iname "*.cpp" -o -iname "*.h"`
do
    echo "Formatting $filename"
    "$CLANG_BINARY" -i "$filename"
done

for filename in `find test -name "argtable3" -prune -o -iname "*.cpp" -o -iname "*.h"`
do
    echo "Formatting $filename"
    "$CLANG_BINARY" -i "$filename"
done