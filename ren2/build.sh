#!/bin/bash

if [ -x /opt/local/bin/g++-mp-4.6 ]; then
    GXX=/opt/local/bin/g++-mp-4.6
else
    GXX=g++
fi
CC="$GXX -std=c++0x -Wall -Werror -I."

set -e
mkdir -p bin
$CC -o bin/Skinning.exe test/Skinning.cpp
$CC -o bin/Passthrough.exe test/Passthrough.cpp
$CC -o bin/Transform.exe test/Transform.cpp
