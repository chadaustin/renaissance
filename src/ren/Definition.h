#ifndef REN_DEFINITION_H
#define REN_DEFINITION_H


#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "Node.h"
#include "Types.h"


namespace ren {

    struct Argument {
        string name;
    };
    typedef std::vector<Argument> ArgumentList;


    struct Definition {
        void print();

        string name;
        std::vector<string> arguments;
        SyntaxNodePtr expression;
    };
    typedef boost::shared_ptr<Definition> DefinitionPtr;

}


#endif
