#ifndef REN_COMPILER_H
#define REN_COMPILER_H


#include "Program.h"
#include "Types.h"


namespace ren {

    ProgramPtr analyze(const string& source);

    struct CompilerResult {
        string vertexShader;
        string fragmentShader;
    };

    CompilerResult compile(const string& source);

}

#endif
