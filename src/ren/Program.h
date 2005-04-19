#ifndef REN_PROGRAM_H
#define REN_PROGRAM_H


#include <stdexcept>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "Definition.h"


namespace ren {

    struct DefinitionInstance {
        Type type;
        SyntaxNodePtr expression;
    };
    typedef boost::shared_ptr<DefinitionInstance> DefinitionInstancePtr;

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

        DefinitionPtr getDefinition(const string& name) {
            for (size_t i = 0; i < definitions.size(); ++i) {
                if (definitions[i]->name == name) {
                    return definitions[i];
                }
            }
            return DefinitionPtr();
        }

        DefinitionInstancePtr resolveDefinition(const string& name /*args*/) {
            DefinitionPtr d = getDefinition(name);
            if (!d) {
                return DefinitionInstancePtr();
            }
            if (d->arguments.size() != 0) {
                throw std::runtime_error("... need a test for this error.");
            }

            DefinitionInstancePtr instance(new DefinitionInstance);
            instance->expression = d->expression;
            //instance->type = type;  Figure out the type.
            return instance;
        }

        std::vector<Uniform> uniforms;
        std::vector<DefinitionPtr> definitions;
    };
    typedef boost::shared_ptr<Program> ProgramPtr;

}


#endif
