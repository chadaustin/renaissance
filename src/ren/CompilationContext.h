#ifndef REN_COMPILATION_CONTEXT_H
#define REN_COMPILATION_CONTEXT_H


#include "ConcreteNode.h"
#include "Errors.h"
#include "Program.h"
#include "Scope.h"
#include "Types.h"


namespace ren {

    inline ScopePtr getBuiltIns() {
        return ScopePtr(new Scope);
    }

    class CompilationContext {
    public:
        CompilationContext(ProgramPtr program)
        : _program(program)
        , _builtIns(getBuiltIns()) {
        }

        ConcreteNodePtr instantiate(const string& name) {
            static boost::shared_ptr<TupleType> empty(new TupleType);
            return instantiate(name, empty);
        }

        ConcreteNodePtr instantiate(
            const string& name,
            const boost::shared_ptr<TupleType> argTypes
        ) {
            static boost::shared_ptr<TupleType> empty(new TupleType);

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
                    return ConcreteNodePtr(new ValueNode("ftransform", VEC4, true));
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

            // Find a definition.
            DefinitionPtr d = _program->getDefinition(name);
            if (!d) {
                throw CompileError(name + " not defined.");
            }

            if (d->arguments.size() != 0) {
                throw CompileError(name + ": arguments not supported.");
            }

            return instantiate(d->expression);
        }

        ConcreteNodePtr instantiate(SyntaxNodePtr syntaxNode) {
            std::vector<ConcreteNodePtr> children;

            for (size_t i = 0; i < syntaxNode->children.size(); ++i) {
                children.push_back(instantiate(syntaxNode->children[i]));
            }

            std::vector<TypeObjectPtr> childrenTypes;
            for (size_t i = 0; i < children.size(); ++i) {
                childrenTypes.push_back(children[i]->getType().get());
            }

            boost::shared_ptr<TupleType> types(new TupleType(childrenTypes));
            ConcreteNodePtr root(instantiate(syntaxNode->name, types));

            if (children.empty()) {
                return root;
            } else {
                return ConcreteNodePtr(new ApplicationNode(root, children));
            }
        }

    private:
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


        ProgramPtr _program;
        ScopePtr _builtIns;
        ScopePtr _programScope;
    };

}


#endif
