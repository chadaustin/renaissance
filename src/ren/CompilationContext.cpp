#include "CompilationContext.h"
#include "Errors.h"
#include "ProgramScope.h"


namespace ren {

    ConcreteNodePtr CompilationContext::instantiate(
        const string& name,
        Type argTypes
    ) {
        ScopePtr programScope(ProgramScope::create(_program));
        if (ConcreteNodePtr cn = programScope->lookup(name, argTypes)) {
            //std::cerr << "Found " << name << "\n";
            return cn;
        }
        //std::cerr << "Didn't find " << name << "\n";
        
        // We didn't find it.
        std::ostringstream os;
        os << name << " accepting " << argTypes.getName()
           << " not defined.";
        throw CompileError(os.str());
    }

}
