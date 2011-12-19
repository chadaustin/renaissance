#!/bin/bash

set -e
g++ -std=c++0x -Wall -Werror -I. -o bin/Transform.exe test/Transform.cpp
#g++ -Wall -Werror -I. -o bin/Skinning.exe test/Skinning.cpp
