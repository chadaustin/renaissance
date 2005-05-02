#ifndef REN_COMPILATION_CONTEXT_H
#define REN_COMPILATION_CONTEXT_H


#include <sstream>
#include "ConcreteNode.h"
#include "Errors.h"
#include "Program.h"
#include "Types.h"


namespace ren {

    class CompilationContext {
    public:
        CompilationContext(ProgramPtr program)
        : _program(program) {
        }

        ConcreteNodePtr instantiate(const string& name) {
            return instantiate(name, NullType);
        }

        ConcreteNodePtr instantiate(const string& name, Type argTypes);

    private:
        ProgramPtr _program;
    };

}


#endif
