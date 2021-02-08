#!/bin/bash
DIR="./lib/stk/"

if [ -d "$DIR" ]; then
    echo "Compiling STK"
    cd ./lib/stk
    autoconf
    ./configure --with-alsa
    cd ./src
    make
else
    echo "STK not found"
fi
