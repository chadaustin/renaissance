#ifndef REN_COMPILER_H
#define REN_COMPILER_H


#include "Types.h"


namespace ren {

    struct CompilerResult {
        string vertexShader;
        string fragmentShader;
    };

    CompilerResult compile(const string& source);

}

#endif
