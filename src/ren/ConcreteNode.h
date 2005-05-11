#ifndef REN_CONCRETE_NODE_H
#define REN_CONCRETE_NODE_H


#include <sstream>
#include <boost/shared_ptr.hpp>
#include "Errors.h"
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
        //virtual Frequency getFrequency() = 0;
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

    private:
        Type _type;
    };
    REN_SHARED_PTR(ArgumentNode);


    class FunctionNode : public ConcreteNode {
    public:
        enum CallType {
            SWIZZLE,
            INFIX,
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
                  InputType inputType = BUILTIN,
                  bool isFunction = false)
        : _name(name)
        , _type(type)
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

        InputType getInputType() const {
            return _inputType;
        }

        const string evaluate() const {
            return _name + (_isFunction ? "()" : "");
        }

    private:
        string _name;
        Type _type;
        InputType _inputType;
        bool _isFunction;
    };
    REN_SHARED_PTR(ValueNode);

}


#endif
