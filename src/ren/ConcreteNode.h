#ifndef REN_CONCRETE_NODE_H
#define REN_CONCRETE_NODE_H


#include <boost/shared_ptr.hpp>
#include "Errors.h"
#include "Types.h"


namespace ren {

    class ConcreteNode;
    typedef boost::shared_ptr<ConcreteNode> ConcreteNodePtr;
    typedef std::vector<ConcreteNodePtr> ConcreteNodeList;


    class ConcreteNode {
    public:
        // Hush gcc.
        virtual ~ConcreteNode() {
        }

        virtual Type getType() const = 0;
        //virtual Frequency getFrequency() = 0;

        virtual const string evaluate() const = 0;
        virtual const string apply(ConcreteNodeList arguments) const = 0;


#if 0
        string evaluate() const {
            if (isFunction(name)) {
                return name + "()";
            } else {
                if (name == "*") {
                    assert(children.size() == 2);
                    SyntaxNodePtr lhs(children[0]);
                    SyntaxNodePtr rhs(children[1]);
                    return paren(lhs->evaluate()) + " " + name + " " +
                           paren(rhs->evaluate());
                } else {
                    return name;
                }
            }
        }

        Type getType() const {
            if (children.empty()) {
                return typeOf(name);
            } else {
                if (name == "*") {
                    assert(children.size() == 2);
                    if (children[0]->getType() == MAT4 &&
                        children[1]->getType() == VEC4
                    ) {
                        return VEC4;
                    }
                }
                if (name == "+") {
                    assert(children.size() == 2);
                    SyntaxNodePtr lhs(children[0]);
                    SyntaxNodePtr rhs(children[1]);
                    if (lhs->getType() == rhs->getType()) {
                        return lhs->getType();
                    }
                }
                if (name == "++") {
                    return VEC2;
                }
                if (name == ".") {
                    return VEC4;
                }
                throw std::runtime_error("Unknown type");
            }
        }

        static Type typeOf(const string& name) {
            if (isInteger(name)) return INT;
            if (isFloat(name))   return FLOAT;
            if (name == "true")  return BOOL;
            if (name == "false") return BOOL;
            if (isBuiltIn(name)) return getBuiltInType(name);
            return VEC4;
        }

        static bool isBuiltIn(const string& name) {
            for (size_t i = 0; i < ARRAY_SIZE(builtIns); ++i) {
                if (builtIns[i].name == name) {
                    return true;
                }
            }
            return false;
        }

        static bool isFunction(const string& name) {
            for (size_t i = 0; i < ARRAY_SIZE(builtIns); ++i) {
                if (builtIns[i].name == name && builtIns[i].function) {
                    return true;
                }
            }
            return false;
        }

        static Type getBuiltInType(const string& name) {            
            for (size_t i = 0; i < ARRAY_SIZE(builtIns); ++i) {
                if (builtIns[i].name == name) {
                    return builtIns[i].type;
                }
            }
            throw CompileError("Unknown built-in type!");
        }
#endif

    };


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

        const string evaluate() const {
            return _op;
        }

        const string apply(ConcreteNodeList arguments) const {
            assert(arguments.size() == 2);
            return "(" + arguments[0]->evaluate() + " " + _op + " " +
                arguments[1]->evaluate() + ")";
        }

    private:
        string _op;
        Type _type;
    };


    class ValueNode : public ConcreteNode {
    public:
        ValueNode(const string& name, Type type, bool isFunction = false)
        : _name(name)
        , _type(type)
        , _isFunction(isFunction) {
        }

        Type getType() const {
            return _type;
        }

        const string evaluate() const {
            return _name + (_isFunction ? "()" : "");
        }

        const string apply(ConcreteNodeList arguments) const {
            throw CompileError("Can't call " + _name + " as function");
        }

    private:
        string _name;
        Type _type;
        bool _isFunction;
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

        const string evaluate() const {
            return _function->apply(_arguments);
        }

        const string apply(ConcreteNodeList arguments) const {
            throw CompileError("I don't know what this means.");
        }

    private:
        ConcreteNodePtr _function;
        ConcreteNodeList _arguments;
    };

}


#endif
