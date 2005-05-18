#include <sstream>
#include "CompilationContext.h"
#include "Errors.h"
#include "ProgramScope.h"


namespace ren {

    CompilationContext::CompilationContext(ProgramPtr program)
    : _scope(ProgramScope::create(program)) {
    }

    ConcreteNodePtr CompilationContext::instantiate(
        const string& name,
        Type argTypes
    ) {
        if (ConcreteNodePtr cn = _scope->lookup(name, argTypes)) {
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


    typedef std::map<ConcreteNodePtr, ConcreteNodePtr> ReplacementMap;


    ConcreteNodePtr copyAndReplace(
        ConcreteNodePtr node,
        const ReplacementMap& rm
    ) {
        ReplacementMap::const_iterator fi = rm.find(node);
        if (fi != rm.end()) {
            return fi->second;
        }

        if (REN_DYNAMIC_CAST(p, ApplicationNode*, node.get())) {

            ConcreteNodePtr function = copyAndReplace(p->getFunction(), rm);
            ConcreteNodeList arguments = p->getArguments();
            for (size_t i = 0; i < arguments.size(); ++i) {
                arguments[i] = copyAndReplace(arguments[i], rm);
            }

            return ConcreteNodePtr(new ApplicationNode(function, arguments));
                    
        } else if (REN_DYNAMIC_CAST(p, AbstractionNode*, node.get())) {

            return ConcreteNodePtr(
                new AbstractionNode(
                    p->getReplacements(),
                    copyAndReplace(p->getInside(), rm)));

        } else if (REN_DYNAMIC_CAST(p, ArgumentNode*, node.get())) {
            (void)p;
            return node;
        } else if (REN_DYNAMIC_CAST(p, FunctionNode*, node.get())) {
            (void)p;
            return node;
        } else if (REN_DYNAMIC_CAST(p, ValueNode*, node.get())) {
            (void)p;
            return node;
        } else {
            assert(!"ICE: Unknown Concrete Node Type");
            return node;
        }
    }


    CodeNodePtr CompilationContext::evaluate(ConcreteNodePtr c) {
        if (_evaluationCache[c]) {
            return _evaluationCache[c];
        }

        if (REN_DYNAMIC_CAST_PTR(v, ValueNode, c)) {
            return cache(c, CodeNodePtr(new NameCodeNode(
                                            v->evaluate(),
                                            v->getType(),
                                            v->getFrequency(),
                                            v->getInputType())));
        } else if (REN_DYNAMIC_CAST_PTR(a, ApplicationNode, c)) {

            ConcreteNodePtr function = a->getFunction();
            const ConcreteNodeList& arguments = a->getArguments();

            if (REN_DYNAMIC_CAST_PTR(f, FunctionNode, function)) {

                CodeNodeList args;
                for (size_t i = 0; i < arguments.size(); ++i) {
                    args.push_back(evaluate(arguments[i]));
                }
                return cache(c, CodeNodePtr(new CallCodeNode(
                                                c->getType(),
                                                f->getCallType(),
                                                f->getName(),
                                                args)));

            } else if (REN_DYNAMIC_CAST_PTR(ab, AbstractionNode, function)) {

                ConcreteNodePtr inside = ab->getInside();
                const ConcreteNodeList& replacements = ab->getReplacements();

                assert(arguments.size() == replacements.size());

                ReplacementMap rm;
                for (size_t i = 0; i < arguments.size(); ++i) {
                    rm[replacements[i]] = arguments[i];
                }

                // clone inside, replacing arguments
                return cache(c, evaluate(copyAndReplace(inside, rm)));

            } else if (REN_DYNAMIC_CAST_PTR(in, IfNode, function)) {

                if (arguments.size() != 3) {
                    throw CompileError("if construct requires exactly 3 arguments.");
                }
                CodeNodePtr condition(evaluate(arguments[0]));
                CodeNodePtr truePart (evaluate(arguments[1]));
                CodeNodePtr falsePart(evaluate(arguments[2]));
                assert(truePart->getType() == falsePart->getType());
                return cache(c, CodeNodePtr(new IfCodeNode(
                                                truePart->getType(),
                                                condition,
                                                truePart,
                                                falsePart)));

            } else {
                assert(!"Error Unknown Function Type!");
                return CodeNodePtr();
            }

        } else if (REN_DYNAMIC_CAST(p, ArgumentNode*, c.get())) {
            assert(!"Can't directly evaluate an ArgumentNode");
            (void)p;
            return CodeNodePtr();
        } else if (REN_DYNAMIC_CAST(p, FunctionNode*, c.get())) {
            assert(!"Can't directly evaluate a FunctionNode");
            (void)p;
            return CodeNodePtr();
        } else {
            //std::cout << typeid(c.get()).name() << std::endl;
            assert(!"Error Unknown ConcreteNode!");
            return CodeNodePtr();
        }
    }

}
