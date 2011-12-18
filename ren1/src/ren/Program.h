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


    class Program {
    public:
        typedef std::vector<Input> InputList;
        typedef std::vector<ValuePtr> ValueList;

        void print();

        // Static Program data.

        void addConstant(const Input& c) {
            _constants.push_back(c);

            size_t index = _constantValues.size();
            _constantValues.push_back(Value::create(c.getType()));
            _constantValueIndices[c.getName()] = index;
        }

        void addUniform(const Input& u) {
            _uniforms.push_back(u);

            size_t index = _uniformValues.size();
            _uniformValues.push_back(Value::create(u.getType()));
            _uniformValueIndices[u.getName()] = index;
        }

        const InputList& getConstants() const {
            return _constants;
        }

        const InputList& getUniforms() const {
            return _uniforms;
        }

        const Input* getConstant(const string& name) const {
            return getInput(name, _constants);
        }

        const Input* getUniform(const string& name) const {
            return getInput(name, _uniforms);
        }

        const Input* getAttribute(const string& name) const {
            return getInput(name, attributes);
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

        // Program Value data.

        ValuePtr getConstantValue(const string& name) {
            size_t index = _constantValueIndices[name];
            assert(index < _constantValues.size());
            return _constantValues[index];
        }

        void setConstantValue(const string& name, ValuePtr v) {
            size_t index = _constantValueIndices[name];
            assert(index < _constantValues.size());
            _constantValues[index] = v;
        }

        const ValueList& getConstantValues() const {
            return _constantValues;
        }

        ValuePtr getUniformValue(const string& name) {
            size_t index = _uniformValueIndices[name];
            assert(index < _uniformValues.size());
            return _uniformValues[index];
        }

        void setUniformValue(const string& name, ValuePtr v) {
            size_t index = _uniformValueIndices[name];
            assert(index < _uniformValues.size());
            _uniformValues[index] = v;
        }

        const ValueList& getUniformValues() const {
            return _uniformValues;
        }

        // Public properties.

        InputList attributes;
        std::vector<DefinitionPtr> definitions;

    private:
        static const Input* getInput(
            const string& name,
            const InputList& inputs
        ) {
            for (size_t i = 0; i < inputs.size(); ++i) {
                if (inputs[i].getName() == name) {
                    return &inputs[i];
                }
            }
            return 0;
        }

        InputList _constants;
        InputList _uniforms;

        typedef std::map<string, size_t> ValueIndexMap;

        ValueIndexMap _constantValueIndices;
        ValueList     _constantValues;

        ValueIndexMap _uniformValueIndices;
        ValueList     _uniformValues;
    };
    typedef boost::shared_ptr<Program> ProgramPtr;

}


#endif
