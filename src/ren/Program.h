#ifndef REN_PROGRAM_H
#define REN_PROGRAM_H


#include <vector>
#include <boost/shared_ptr.hpp>
#include "Definition.h"


namespace ren {

    struct Program {
        void print();

        void inferTypes() {
            for (size_t i = 0; i < definitions.size(); ++i) {
                definitions[i]->inferType();
            }
        }

        DefinitionPtr getDefinition(const string& name) {
            for (size_t i = 0; i < definitions.size(); ++i) {
                if (definitions[i]->name == name) {
                    return definitions[i];
                }
            }
            return DefinitionPtr();
        }

        std::vector<DefinitionPtr> definitions;
    };
    typedef boost::shared_ptr<Program> ProgramPtr;

}


#endif
