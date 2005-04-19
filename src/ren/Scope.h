#ifndef REN_SCOPE_H
#define REN_SCOPE_H


#include <boost/shared_ptr.hpp>


namespace ren {

    class Scope;
    typedef boost::shared_ptr<Scope> ScopePtr;

    class Scope {
        // lookup symbols

        ScopePtr parent;
    };

}


#endif
