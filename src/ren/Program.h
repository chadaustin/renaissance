#ifndef REN_PROGRAM_H
#define REN_PROGRAM_H


#include <map>
#include <stdexcept>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "Definition.h"
#include "Types.h"
#include "Value.h"


namespace ren {

    struct Input {
    public:
        Input(Type type, string name)
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


    typedef Input Constant;
    typedef Input Uniform;
    typedef Input Attribute;


    struct Program {
        void print();

        const Constant* getConstant(const string& name) const {
            for (size_t i = 0; i < constants.size(); ++i) {
                if (constants[i].getName() == name) {
                    return &constants[i];
                }
            }
            return 0;
        }

        const Uniform* getUniform(const string& name) const {
            for (size_t i = 0; i < uniforms.size(); ++i) {
                if (uniforms[i].getName() == name) {
                    return &uniforms[i];
                }
            }
            return 0;
        }

        const Attribute* getAttribute(const string& name) const {
            for (size_t i = 0; i < attributes.size(); ++i) {
                if (attributes[i].getName() == name) {
                    return &attributes[i];
                }
            }
            return 0;
        }

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

        ValuePtr getConstantValue(const string& name, Type type) {
            if (_constantValues[name]) {
                return _constantValues[name];
            } else {
                ValuePtr v = Value::create(type);
                _constantValues[name] = v;
                return v;
            }
        }

        void setConstantValue(const string& name, ValuePtr v) {
            _constantValues[name] = v;
        }

        std::vector<Constant> constants;
        std::vector<Uniform> uniforms;
        std::vector<Attribute> attributes;
        std::vector<DefinitionPtr> definitions;

    private:
        std::map<string, ValuePtr> _constantValues;
    };
    typedef boost::shared_ptr<Program> ProgramPtr;

}


#endif
