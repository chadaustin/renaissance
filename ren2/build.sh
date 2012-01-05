#!/bin/bash

# can't use clang yet.  no libcxx automatically installed by macports
if [ -x /opt/local/bin/g++-mp-4.6 ]; then
    CXX=/opt/local/bin/g++-mp-4.6
else
    CXX=g++
fi
CC="$CXX -g -std=c++0x -Wall -Werror"

set -e
set -x
mkdir -p bin
#$CC -I. -o ren/ren.h.gch ren/ren.h

function test() {
    $CC -I. -o bin/$1.exe test/$1.cpp
}
test ConstantOutput
test ConditionalSelection
test OptionalMorphing
test OptionalSkinning
test Skinning
test Passthrough
test Transform
test ColorOutput
test TexCoordOutput
test Redundant
