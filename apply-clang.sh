#!/bin/sh

for filename in `find src -iname "*.cpp" -o -iname "*.h"`
do
    echo "Formatting $filename"
    "c:\Program Files\LLVM\bin\clang-format.exe" -i "$filename"
done

for filename in `find test -iname "*.cpp" -o -iname "*.h"`
do
    echo "Formatting $filename"
    "c:\Program Files\LLVM\bin\clang-format.exe" -i "$filename"
done