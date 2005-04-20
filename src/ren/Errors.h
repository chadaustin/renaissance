#ifndef REN_ERRORS_H
#define REN_ERRORS_H


#include <stdexcept>
#include "Types.h"


namespace ren {

    class CompileError : public std::runtime_error {
    public:
        CompileError(const string& text)
        : std::runtime_error(text) { }
    };

}


#endif
