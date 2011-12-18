#ifndef REN_BUILT_IN_SCOPE_H
#define REN_BUILT_IN_SCOPE_H


#include "Scope.h"


namespace ren {

    class BuiltInScope : public Scope {
    public:
        ConcreteNodePtr lookup(const string& name, Type argTypes);
    };

}


#endif
