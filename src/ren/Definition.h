#ifndef REN_DEFINITION_H
#define REN_DEFINITION_H


#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "SyntaxNode.h"
#include "Types.h"


namespace ren {

    typedef std::vector<string> ArgumentList;


    struct Definition {
        void print();

        string name;
        ArgumentList arguments;
        SyntaxNodePtr expression;
    };
    typedef boost::shared_ptr<Definition> DefinitionPtr;

}


#endif
