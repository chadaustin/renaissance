#ifndef REN_PROGRAM_H
#define REN_PROGRAM_H


#include <stdexcept>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "Definition.h"
#include "Types.h"


namespace ren {

    struct Uniform {
    public:
        Uniform(Type type, string name)
        : _type(type)
        , _name(name) {
        }

        Type getType() const {
            return _type;
        }

        string getName() const {
            return _name;
        }

    private:
        Type _type;
        string _name;
    };


    struct Program {
        void print();

        DefinitionPtr getDefinition(
            const string& name,
            size_t args = 0
        ) const {
            for (size_t i = 0; i < definitions.size(); ++i) {
                if (definitions[i]->name == name &&
                    definitions[i]->arguments.size() == args
                ) {
                    return definitions[i];
                }
            }
            return DefinitionPtr();
        }

        bool hasDefinition(const string& name) const {
            return getDefinition(name);
        }

        const Uniform* getUniform(const string& name) const {
            for (size_t i = 0; i < uniforms.size(); ++i) {
                if (uniforms[i].getName() == name) {
                    return &uniforms[i];
                }
            }
            return 0;
        }

        std::vector<Uniform> uniforms;
        std::vector<DefinitionPtr> definitions;
    };
    typedef boost::shared_ptr<Program> ProgramPtr;

}


#endif
