#!/bin/bash

CC="g++ -std=c++0x -Wall -Werror -I."

set -e
$CC -o bin/Passthrough.exe test/Passthrough.cpp
$CC -o bin/Transform.exe test/Transform.cpp
#g++ -std=c++0x -Wall -Werror -I. -o bin/Skinning.exe test/Skinning.cpp
