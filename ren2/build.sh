#!/bin/bash

# can't use clang yet.  no libcxx automatically installed by macports
if [ -x /opt/local/bin/g++-mp-4.6 ]; then
    CXX=/opt/local/bin/g++-mp-4.6
else
    CXX=g++
fi
CC="$CXX -g -std=c++0x -Wall -Werror -I."

set -e
mkdir -p bin
$CC -o bin/ConditionalSelection.exe test/ConditionalSelection.cpp
$CC -o bin/OptionalSkinning.exe test/OptionalSkinning.cpp
$CC -o bin/Skinning.exe test/Skinning.cpp
$CC -o bin/Passthrough.exe test/Passthrough.cpp
$CC -o bin/Transform.exe test/Transform.cpp
$CC -o bin/ColorOutput.exe test/ColorOutput.cpp
$CC -o bin/TexCoordOutput.exe test/TexCoordOutput.cpp
$CC -o bin/Redundant.exe test/Redundant.cpp
