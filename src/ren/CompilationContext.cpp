#include <map>
#include <iostream>
#include <boost/enable_shared_from_this.hpp>
#include "CompilationContext.h"
#include "Scope.h"


namespace ren {


#if 0
    struct BuiltIn {
        string name;
        Type type;
        bool function;
    };

    static BuiltIn builtIns[] = {
        // Default attributes.
        { "gl_Color",          VEC4   },
        { "gl_SecondaryColor", VEC4   },
        { "gl_Normal",         VEC3   },
        { "gl_Vertex",         VEC4   },
        { "gl_FogCoord",       FLOAT  },
        //{ "gl_MultiTexCoord",  "vec4[]" },

        // ftransform is kind of a special attribute in that it's a function call...
        { "ftransform",        VEC4, true },

        // Default varyings.

        // Default uniforms.

        // Built-in state.
        { "gl_ModelViewProjectionMatrix", MAT4 },
    };


#define ARRAY_SIZE(array) (sizeof(array) / sizeof(*(array)))
#endif


    static bool isInteger(const string& name) {
        for (size_t i = 0; i < name.size(); ++i) {
            if (!isdigit(name[i])) {
                return false;
            }
        }
        return true;
    }


    static bool isFloat(const string& name) {
        bool has_period = false;
        for (size_t i = 0; i < name.size(); ++i) {
            if (name[i] == '.') {
                if (has_period) {
                    return false;
                }
                has_period = true;
            } else if (!isdigit(name[i])) {
                return false;
            }
        }
        return has_period;
    }


    class BuiltInScope : public Scope {
    public:
        ConcreteNodePtr lookup(
            const string& name,
            const TupleTypePtr& argTypes
        ) {
            static TupleTypePtr empty(new TupleType);

            if (name == "true" || name == "false") {
                if (Type(argTypes) == Type(empty)) {
                    return ConcreteNodePtr(new ValueNode(name, BOOL));
                } else {
                    throw CompileError("Can't call a boolean.");
                }
            }

            if (isInteger(name)) {
                if (Type(argTypes) == Type(empty)) {
                    return ConcreteNodePtr(new ValueNode(name, INT));
                } else {
                    throw CompileError("Can't call an integer.");
                }
            }

            if (isFloat(name)) {
                if (Type(argTypes) == Type(empty)) {
                    return ConcreteNodePtr(new ValueNode(name, FLOAT));
                } else {
                    throw CompileError("Can't call a float.");
                }
            }

            if (name == "*") {
                Type fxf(makeTupleType(MAT4, VEC4));
                if (Type(argTypes) == fxf) {
                    Type ft(makeFunctionType(fxf, VEC4));
                    return ConcreteNodePtr(new InfixNode("*", ft));
                } else {
                    throw CompileError(name + ": Uh oh.  Can't do that.");
                }
            }

            if (name == "+") {
                Type ixi(makeTupleType(INT, INT));
                if (Type(argTypes) == ixi) {
                    Type ft(makeFunctionType(ixi, INT));
                    return ConcreteNodePtr(new InfixNode("+", ft));
                } else {
                    throw CompileError(name + ": Uh oh.  Can't do that.");
                }
            }

            if (name == "ftransform") {
                if (Type(argTypes) == Type(empty)) {
                    return ConcreteNodePtr(new ValueNode(
                                               "ftransform",
                                               VEC4,
                                               ValueNode::BUILTIN,
                                               true));
                } else {
                    throw CompileError(name + ": Uh oh.  Can't do that.");
                }
            }

            if (name == "gl_Vertex") {
                if (Type(argTypes) == Type(empty)) {
                    return ConcreteNodePtr(new ValueNode("gl_Vertex", VEC4));
                } else {
                    throw CompileError(name + ": Uh oh.  Can't do that.");
                }
            }

            if (name == "gl_Color") {
                if (Type(argTypes) == Type(empty)) {
                    return ConcreteNodePtr(new ValueNode("gl_Color", VEC4));
                } else {
                    throw CompileError(name + ": Uh oh.  Can't do that.");
                }
            }

            if (name == "gl_ModelViewProjectionMatrix") {
                if (Type(argTypes) == Type(empty)) {
                    return ConcreteNodePtr(new ValueNode("gl_ModelViewProjectionMatrix", MAT4));
                } else {
                    throw CompileError(name + ": Uh oh.  Can't do that.");
                }
            }

            if (name == "xyz") {
                if (Type(argTypes) == makeTupleType(VEC4)) {
                    Type ft(makeFunctionType(makeTupleType(VEC4), VEC3));
                    return ConcreteNodePtr(new ValueNode("xyx", ft));
                }
            }

            if (name == "xy") {
                if (Type(argTypes) == makeTupleType(VEC4)) {
                    Type ft(makeFunctionType(makeTupleType(VEC4), VEC2));
                    return ConcreteNodePtr(new ValueNode("xy", ft));
                }
            }

            return ConcreteNodePtr();
        }
    };


    class InnerScope : public Scope {
    public:
        typedef std::map<string, ConcreteNodePtr> SymbolMap;

        InnerScope(ScopePtr parent, const SymbolMap& symbols)
        : _parent(parent)
        , _symbols(symbols) {
        }

        ConcreteNodePtr lookup(
            const string& name,
            const TupleTypePtr& argTypes
        ) {
            if (_symbols.count(name)) {
                return _symbols[name];
            }

            return _parent->lookup(name, argTypes);
        }

    private:
        ScopePtr _parent;
        SymbolMap _symbols;
    };


    class ProgramScope
        : public Scope
        , public boost::enable_shared_from_this<ProgramScope> {
    private:
        ProgramScope(ProgramPtr program)
        : _program(program) {
        }

    public:
        typedef boost::shared_ptr<ProgramScope> Ptr;

        static Ptr create(ProgramPtr program) {
            return Ptr(new ProgramScope(program));
        }

        ConcreteNodePtr lookup(
            const string& name,
            const TupleTypePtr& argTypes
        ) {
            static Type empty(new TupleType);

            //std::cerr << "Instantiating name: " << name << " " << Type(argTypes) << "\n";

            // Is it a uniform?
            const Uniform* u = _program->getUniform(name);
            if (u) {
                return ConcreteNodePtr(
                    new ValueNode(
                        name,
                        u->getType(),
                        ValueNode::UNIFORM));
            }

            // Find a definition.
            DefinitionPtr d = _program->getDefinition(
                name,
                argTypes->size());
            if (d) {
                //std::cerr << "...definition\n";

                if (Type(argTypes) == empty) {
                    return instantiate(d->expression, shared_from_this());
                } else {
                    // create new abstraction node, pass in a scope.
                    InnerScope::SymbolMap symbols;
                    for (size_t i = 0; i < d->arguments.size(); ++i) {
                        ConcreteNodePtr arg(new ArgumentNode(
                                                argTypes->getType(i)));
                        symbols[d->arguments[i]] = arg;
                    }
                    ScopePtr inner(new InnerScope(shared_from_this(),
                                                  symbols));

                    ConcreteNodePtr cn(instantiate(d->expression, inner));

                    ConcreteNodeList arguments;
                    for (size_t i = 0; i < d->arguments.size(); ++i) {
                        arguments.push_back(symbols[d->arguments[i]]);
                    }
                    return ConcreteNodePtr(new AbstractionNode(arguments, cn));
                }
            }

            //std::cerr << "  Looking in built-ins...\n";
            ConcreteNodePtr cn = BuiltInScope().lookup(name, argTypes);
            //std::cerr << "  Looked...\n";
            return cn;
        }

    private:
        ConcreteNodePtr instantiate(
            SyntaxNodePtr syntaxNode,
            ScopePtr scope
        ) {
            //std::cerr << "Instantiating node: " << syntaxNode->toString() << "\n";

            if (ApplySyntaxNode* p =
                dynamic_cast<ApplySyntaxNode*>(syntaxNode.get())) {

                //std::cerr << "  Apply\n";

                assert(!p->getChildren().empty());
                SyntaxNodePtr rootSExp(p->getChildren()[0]);

                std::vector<ConcreteNodePtr> children;
                for (size_t i = 1; i < p->getChildren().size(); ++i) {
                    ConcreteNodePtr c = instantiate(p->getChildren()[i],
                                                    scope);
                    children.push_back(c);
                }
                assert(!children.empty());

                std::vector<TypeObjectPtr> childrenTypes;
                for (size_t i = 0; i < children.size(); ++i) {
                    childrenTypes.push_back(children[i]->getType().get());
                }

                ValueSyntaxNode* v = dynamic_cast<ValueSyntaxNode*>(
                    rootSExp.get());
                assert(v);

                TupleTypePtr types(new TupleType(childrenTypes));
                ConcreteNodePtr root(scope->lookup(v->getName(), types));
                if (!root) {
                    std::ostringstream os;
                    os << v->getName() << " accepting "
                       << types->getName() << " not defined.";
                    throw CompileError(os.str());
                }

                return ConcreteNodePtr(new ApplicationNode(root, children));

            } else if (ValueSyntaxNode* p =
                       dynamic_cast<ValueSyntaxNode*>(syntaxNode.get())) {

                //std::cerr << "  Value\n";

                // Assume that we're "calling" a function with no arguments.
                static TupleTypePtr empty(new TupleType);
                ConcreteNodePtr cn(scope->lookup(p->getName(), empty));
                if (!cn) {
                    std::ostringstream os;
                    os << p->getName() << " not defined.";
                    throw CompileError(os.str());
                }
                return cn;
            
            } else {
                assert(!"Syntax node is of unknown type");
                return ConcreteNodePtr();
            }
        }

        ProgramPtr _program;
    };


    ConcreteNodePtr CompilationContext::instantiate(
        const string& name,
        const TupleTypePtr& argTypes
    ) {
        ScopePtr programScope(ProgramScope::create(_program));
        if (ConcreteNodePtr cn = programScope->lookup(name, argTypes)) {
            //std::cerr << "Found " << name << "\n";
            return cn;
        }
        //std::cerr << "Didn't find " << name << "\n";
        
        // We didn't find it.
        std::ostringstream os;
        os << name << " accepting " << argTypes->getName()
           << " not defined.";
        throw CompileError(os.str());
    }

}
