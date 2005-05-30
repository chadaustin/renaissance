#ifndef REN_COMPILER_H
#define REN_COMPILER_H


#include <iostream>
#include "Program.h"
#include "Types.h"


namespace ren {

    ProgramPtr parse(const string& source);

    struct CompileResult {
        CompileResult(bool s, const string& vs = "", const string& fs = "")
        : success(s)
        , vertexShader(vs)
        , fragmentShader(fs) {
        }

        bool success;
        string vertexShader;
        string fragmentShader;
    };

    CompileResult compile(ProgramPtr program,
                          std::ostream& output = std::cerr);
    CompileResult compile(const string& source,
                          std::ostream& output = std::cerr);

}

#endif
