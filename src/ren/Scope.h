#ifndef REN_SCOPE_H
#define REN_SCOPE_H


#include <boost/shared_ptr.hpp>
#include "Types.h"


namespace ren {

    class Scope {
    public:
        virtual ~Scope() { }

        virtual ConcreteNodePtr lookup(
            const string& name,
            const TupleTypePtr& argTypes) = 0;
    };
    REN_SHARED_PTR(Scope);

}


#endif
