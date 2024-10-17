#!/bin/bash

PLATFORM=${1:-seed}
PATCH=${2:-blink}

cd "patches/$PLATFORM"
if [ $? -ne 0 ]; then
    printf "Cannot find platform directory \"$PWD/patches/$PLATFORM\"\n"
    exit 1
fi

cd "$PATCH"
if [ $? -ne 0 ]; then
    printf "Cannot find patch \"$PATCH\"\n"
    exit 1
fi

make clean -s
make -s
make -s program-dfu

printf "\nDone.\n"
