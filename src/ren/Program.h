#ifndef REN_PROGRAM_H
#define REN_PROGRAM_H


#include <stdexcept>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "Definition.h"
#include "Types.h"


namespace ren {

    struct Uniform {
        Uniform(Type type_, string name_)
        : type(type_)
        , name(name_) {
        }

        Type type;
        string name;
    };


    struct Program {
        void print();

        DefinitionPtr getDefinition(const string& name) const {
            for (size_t i = 0; i < definitions.size(); ++i) {
                if (definitions[i]->name == name) {
                    return definitions[i];
                }
            }
            return DefinitionPtr();
        }

        bool hasDefinition(const string& name) const {
            return getDefinition(name);
        }

        std::vector<Uniform> uniforms;
        std::vector<DefinitionPtr> definitions;
    };
    typedef boost::shared_ptr<Program> ProgramPtr;

}


#endif
