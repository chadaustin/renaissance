#ifndef COMPILER_H
#define COMPILER_H


#include "Types.h"


struct CompilerResult {
    string vertexShader;
    string fragmentShader;
};


CompilerResult compile(const string& source);


#endif
