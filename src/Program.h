#ifndef PROGRAM_H
#define PROGRAM_H


#include <vector>
#include <boost/shared_ptr.hpp>
#include "Definition.h"


struct Program {
    void print();

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


#endif
