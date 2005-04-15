#ifndef REN_PROGRAM_H
#define REN_PROGRAM_H


#include <stdexcept>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "Definition.h"


namespace ren {

    struct Instance {
        Type type;
        NodePtr expression;
    };
    typedef boost::shared_ptr<Instance> InstancePtr;

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

        InstancePtr resolveDefinition(const string& name /*args*/) {
            DefinitionPtr d = getDefinition(name);
            if (!d) {
                return InstancePtr();
            }
            if (d->arguments.size() != 0) {
                throw std::runtime_error("... need a test for this error.");
            }

            InstancePtr instance(new Instance);
            instance->type = d->type;
            instance->expression = d->expression;
            return instance;
        }

        std::vector<Uniform> uniforms;
        std::vector<DefinitionPtr> definitions;
    };
    typedef boost::shared_ptr<Program> ProgramPtr;

}


#endif
