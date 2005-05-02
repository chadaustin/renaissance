#ifndef REN_SCOPE_H
#define REN_SCOPE_H


#include "ConcreteNode.h"
#include "Types.h"


namespace ren {

    class Scope {
    public:
        virtual ~Scope() { }

        virtual ConcreteNodePtr lookup(const string& name, Type argTypes) = 0;
    };
    REN_SHARED_PTR(Scope);

}


#endif
