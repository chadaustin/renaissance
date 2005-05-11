#ifndef REN_COMPILATION_CONTEXT_H
#define REN_COMPILATION_CONTEXT_H


#include <map>
#include "CodeNode.h"
#include "ConcreteNode.h"
#include "Program.h"
#include "Scope.h"
#include "Types.h"


namespace ren {

    class CompilationContext {
    public:
        CompilationContext(ProgramPtr program);

        ConcreteNodePtr instantiate(const string& name) {
            return instantiate(name, NullType);
        }

        ConcreteNodePtr instantiate(const string& name, Type argTypes);

        CodeNodePtr evaluate(ConcreteNodePtr c);

    private:
        ScopePtr _scope;

        CodeNodePtr cache(ConcreteNodePtr key, CodeNodePtr value) {
            assert(key);
            assert(value);
            _evaluationCache[key] = value;
            return value;
        }

        typedef std::map<ConcreteNodePtr, CodeNodePtr> EvaluationCache;
        EvaluationCache _evaluationCache;
    };

}


#endif
