#ifndef REN_CONCRETE_NODE_H
#define REN_CONCRETE_NODE_H


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

        virtual Type getType() const = 0;
        //virtual Frequency getFrequency() = 0;
    };


    class ApplicationNode : public ConcreteNode {
    public:
        ApplicationNode(ConcreteNodePtr function, ConcreteNodeList arguments)
        : _function(function)
        , _arguments(arguments) {
        }

        Type getType() const {
            Type functionType(_function->getType());
            if (const FunctionType* ft = dynamic_cast<const FunctionType*>(
                    functionType.get().get())) {
                return ft->getOutType();
            } else {
                throw CompileError("Calling non-function.");
            }
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
        }

        Type getType() const {
            std::vector<TypeObjectPtr> types;
            for (size_t i = 0; i < _replacements.size(); ++i) {
                types.push_back(_replacements[i]->getType().get());
            }

            Type ft(makeFunctionType(new TupleType(types),
                                     _inside->getType()));
            return ft;
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

        Type getType() const {
            return _type;
        }

    private:
        Type _type;
    };
    REN_SHARED_PTR(ArgumentNode);


    /// Represents an infix operator.
    class InfixNode : public ConcreteNode {
    public:
        InfixNode(const string& op, Type type)
        : _op(op)
        , _type(type) {
        }

        Type getType() const {
            return _type;
        }

        string getOperator() const {
            return _op;
        }

    private:
        string _op;
        Type _type;
    };
    REN_SHARED_PTR(InfixNode);


    class ValueNode : public ConcreteNode {
    public:
        enum InputType {
            BUILTIN,
            UNIFORM,
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
