#ifndef REN_COMPILER_H
#define REN_COMPILER_H


#include <iostream>
#include "Program.h"
#include "Types.h"


namespace ren {

    ProgramPtr parse(const string& source,
                     std::ostream& output = std::cerr);

    ProgramPtr analyze(const string& source,
                       std::ostream& output = std::cerr);

    struct CompilerResult {
        string vertexShader;
        string fragmentShader;
    };

    CompilerResult compile(const string& source,
                           std::ostream& output = std::cerr);

}

#endif
