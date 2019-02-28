#!/bin/sh

CLANG_BINARY="$HOME/.vscode/extensions/ms-vscode.cpptools-0.21.0/LLVM/bin/clang-format.exe"

for filename in `find src -iname "*.cpp" -o -iname "*.h"`
do
    echo "Formatting $filename"
    $CLANG_BINARY -i "$filename"
done

for filename in `find test -iname "*.cpp" -o -iname "*.h"`
do
    echo "Formatting $filename"
    $CLANG_BINARY -i "$filename"
done