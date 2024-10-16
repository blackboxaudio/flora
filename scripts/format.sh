#!/bin/bash

cd patches/
if [ $? -ne 0 ]; then
    printf "Failed to find \"patches\" directory\n"
    exit 1
fi

printf "Formatting code...\n"
find . -iname '*.h' -o -iname '*.cpp' | xargs clang-format -i -style=WebKit
if [ $? -ne 0 ]; then
    printf "Failed to format code\n"
    exit 1
fi

printf "Done.\n"
