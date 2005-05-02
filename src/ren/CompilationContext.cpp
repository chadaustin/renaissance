#include <map>
#include <iostream>
#include <boost/enable_shared_from_this.hpp>
#include "BuiltInScope.h"
#include "CompilationContext.h"
#include "Scope.h"


namespace ren {


    class InnerScope : public Scope {
    public:
        typedef std::map<string, ConcreteNodePtr> SymbolMap;

        InnerScope(ScopePtr parent, const SymbolMap& symbols)
        : _parent(parent)
        , _symbols(symbols) {
        }

        ConcreteNodePtr lookup(const string& name, Type argTypes) {
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

        ConcreteNodePtr lookup(const string& name, Type argTypes) {
            //std::cerr << "Instantiating name: " << name << " " << Type(argTypes) << "\n";

            // Is it a uniform?
            const Uniform* u = _program->getUniform(name);
            if (u && argTypes == NullType) {
                return ConcreteNodePtr(
                    new ValueNode(
                        name,
                        u->getType(),
                        ValueNode::UNIFORM));
            }

            TypeList argTuple(asTuple(argTypes));

            // Find a definition.
            DefinitionPtr d = _program->getDefinition(
                name,
                argTuple.size());
            if (d) {
                //std::cerr << "...definition\n";

                if (argTypes == NullType) {
                    return instantiate(d->expression, shared_from_this());
                } else {
                    // create new abstraction node, pass in a scope.
                    InnerScope::SymbolMap symbols;
                    for (size_t i = 0; i < d->arguments.size(); ++i) {
                        ConcreteNodePtr arg(new ArgumentNode(argTuple[i]));
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

            if (REN_DYNAMIC_CAST(p, ApplySyntaxNode*, syntaxNode.get())) {

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

                TypeList childrenTypes;
                for (size_t i = 0; i < children.size(); ++i) {
                    childrenTypes.push_back(children[i]->getType());
                }

                REN_DYNAMIC_CAST(v, ValueSyntaxNode*, rootSExp.get());
                assert(v);

                Type types(makeTuple(childrenTypes));
                ConcreteNodePtr root(scope->lookup(v->getName(), types));
                if (!root) {
                    std::ostringstream os;
                    os << v->getName() << " accepting "
                       << types.getName() << " not defined.";
                    throw CompileError(os.str());
                }

                return ConcreteNodePtr(new ApplicationNode(root, children));

            } else if (REN_DYNAMIC_CAST(p, ValueSyntaxNode*,
                                        syntaxNode.get())) {

                //std::cerr << "  Value\n";

                // Assume that we're "calling" a function with no arguments.
                ConcreteNodePtr cn(scope->lookup(p->getName(), NullType));
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
        Type argTypes
    ) {
        ScopePtr programScope(ProgramScope::create(_program));
        if (ConcreteNodePtr cn = programScope->lookup(name, argTypes)) {
            //std::cerr << "Found " << name << "\n";
            return cn;
        }
        //std::cerr << "Didn't find " << name << "\n";
        
        // We didn't find it.
        std::ostringstream os;
        os << name << " accepting " << argTypes.getName()
           << " not defined.";
        throw CompileError(os.str());
    }

}
