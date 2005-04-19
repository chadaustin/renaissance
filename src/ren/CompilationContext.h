#ifndef REN_COMPILATION_CONTEXT_H
#define REN_COMPILATION_CONTEXT_H


#include "Program.h"
#include "Scope.h"


namespace ren {

    inline ScopePtr getBuiltIns() {
        return ScopePtr(new Scope);
    }

    class CompilationContext {
    public:
        CompilationContext(ProgramPtr program)
        : _program(p)
        , _builtIns(getBuiltIns()) {
        }

        

    private:
        ProgramPtr _program;
        ScopePtr _builtIns;
        ScopePtr _programScope;
    };

}


#endif
