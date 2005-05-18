#ifndef REN_CONCRETE_NODE_H
#define REN_CONCRETE_NODE_H


#include <sstream>
#include <boost/shared_ptr.hpp>
#include "Errors.h"
#include "Frequency.h"
#include "Types.h"


namespace ren {

    class ConcreteNode;
    REN_SHARED_PTR(ConcreteNode);
    typedef std::vector<ConcreteNodePtr> ConcreteNodeList;


    class ConcreteNode {
    public:
        // Hush gcc.
        virtual ~ConcreteNode() {
        }

        virtual string asStringTree() const = 0;

        virtual Type getType() const = 0;
        virtual Frequency getFrequency() const = 0;
    };


    class ApplicationNode : public ConcreteNode {
    public:
        ApplicationNode(ConcreteNodePtr function, ConcreteNodeList arguments)
        : _function(function)
        , _arguments(arguments) {
        }

        string asStringTree() const {
            string rv = "(@ " + _function->asStringTree();
            for (size_t i = 0; i < _arguments.size(); ++i) {
                rv += " " + _arguments[i]->asStringTree();
            }
            return rv + ")";
        }

        Type getType() const {
            return asFunction(_function->getType()).out;
        }

        Frequency getFrequency() const {
            assert(!_arguments.empty());
            Frequency rv = _arguments[0]->getFrequency();
            for (size_t i = 1; i < _arguments.size(); ++i) {
                rv = std::max(rv, _arguments[i]->getFrequency());
            }
            return rv;
        }

        ConcreteNodePtr getFunction() const {
            return _function;
        }

        const ConcreteNodeList& getArguments() const {
            return _arguments;
        }

    private:
        ConcreteNodePtr _function;
        ConcreteNodeList _arguments;
    };
    REN_SHARED_PTR(ApplicationNode);


    class AbstractionNode : public ConcreteNode {
    public:
        AbstractionNode(ConcreteNodeList replacements, ConcreteNodePtr inside)
        : _replacements(replacements)
        , _inside(inside) {
            assert(_replacements.size() >= 1);
        }

        string asStringTree() const {
            string rv = "\\";
            for (size_t i = 0; i < _replacements.size(); ++i) {
                rv += _replacements[i]->asStringTree() + ".";
            }
            return rv + _inside->asStringTree();;
        }

        Type getType() const {
            TypeList types;
            for (size_t i = 0; i < _replacements.size(); ++i) {
                types.push_back(_replacements[i]->getType());
            }

            return makeTuple(types) >> _inside->getType();
        }

        Frequency getFrequency() const {
            return CONSTANT;
        }

        const ConcreteNodeList& getReplacements() const {
            return _replacements;
        }

        ConcreteNodePtr getInside() const {
            return _inside;
        }

    private:
        ConcreteNodeList _replacements;
        ConcreteNodePtr _inside;
    };
    REN_SHARED_PTR(AbstractionNode);


    class ArgumentNode : public ConcreteNode {
    public:
        ArgumentNode(Type type)
        : _type(type) {
        }

        string asStringTree() const {
            std::ostringstream os;
            os << this;
            return os.str();
        }

        Type getType() const {
            return _type;
        }

        Frequency getFrequency() const {
            // Shouldn't matter, since this node gets replaced on
            // substitution.
            return CONSTANT;
        }

    private:
        Type _type;
    };
    REN_SHARED_PTR(ArgumentNode);


    class IfNode : public ConcreteNode {
    public:
        IfNode(Type type)
        : _type(type) {
        }

        string asStringTree() const {
            return "if";
        }

        Type getType() const {
            return _type;
        }

        Frequency getFrequency() const {
            return CONSTANT;
        }

    private:
        Type _type;
    };


    class FunctionNode : public ConcreteNode {
    public:
        enum CallType {
            SWIZZLE,
            INFIX,
            PREFIX,
            FUNCTION,
        };

        FunctionNode(const string& name, Type type, CallType callType)
        : _name(name)
        , _type(type)
        , _callType(callType) {
        }

        string asStringTree() const {
            return _name;
        }

        Type getType() const {
            return _type;
        }

        Frequency getFrequency() const {
            return CONSTANT;
        }

        string getName() const {
            return _name;
        }

        CallType getCallType() const {
            return _callType;
        }

    private:
        string _name;
        Type _type;
        CallType _callType;
    };
    REN_SHARED_PTR(FunctionNode);


    class ValueNode : public ConcreteNode {
    public:
        enum InputType {
            BUILTIN,
            UNIFORM,
            ATTRIBUTE,
        };

        ValueNode(const string& name,
                  Type type,
                  Frequency frequency,
                  InputType inputType = BUILTIN,
                  bool isFunction = false)
        : _name(name)
        , _type(type)
        , _frequency(frequency)
        , _inputType(inputType)
        , _isFunction(isFunction) {
        }

        string getName() const {
            return _name;
        }

        string asStringTree() const {
            return _name;
        }

        Type getType() const {
            return _type;
        }

        Frequency getFrequency() const {
            return _frequency;
        }

        InputType getInputType() const {
            return _inputType;
        }

        const string evaluate() const {
            return _name + (_isFunction ? "()" : "");
        }

    private:
        string _name;
        Type _type;
        Frequency _frequency;
        InputType _inputType;
        bool _isFunction;
    };
    REN_SHARED_PTR(ValueNode);

}


#endif
