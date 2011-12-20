#!/bin/bash

set -e
g++ -std=c++0x -Wall -Werror -I. -o bin/Passthrough.exe test/Passthrough.cpp
#g++ -std=c++0x -Wall -Werror -I. -o bin/Transform.exe test/Transform.cpp
#g++ -std=c++0x -Wall -Werror -I. -o bin/Skinning.exe test/Skinning.cpp
