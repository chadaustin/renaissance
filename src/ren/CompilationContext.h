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
            static TupleTypePtr empty(new TupleType);
            return instantiate(name, empty);
        }

        ConcreteNodePtr instantiate(
            const string& name,
            const TupleTypePtr& argTypes);

    private:
        ProgramPtr _program;
    };

}


#endif
