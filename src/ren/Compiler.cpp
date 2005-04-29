#include <sstream>
#include "CodeNode.h"
#include "CompilationContext.h"
#include "Compiler.h"
#include "ConcreteNode.h"
#include "Errors.h"

#include "ShaderLexer.hpp"
#include "ShaderParser.hpp"
#include "ShaderValidator.hpp"


namespace ren {

    ProgramPtr parse(const string& source) {
        std::istringstream is(source);

        antlr::ASTFactory parserFactory;
        ShaderLexer lexer(is);
        ShaderParser parser(lexer);
        parser.initializeASTFactory(parserFactory);
        parser.setASTFactory(&parserFactory);
        parser.program();

        if (antlr::RefAST ast = parser.getAST()) {
            ShaderValidator validator;
            return validator.program(ast);
        } else {
            // Parser didn't build an AST or throw an error.  Thus,
            // the program is empty.
            return ProgramPtr(new Program);
        }

    }


    inline void requireType(
        const string& name,
        ConcreteNodePtr node,
        Type type
    ) {
        if (node->getType() != type) {
            throw CompileError(
                name + " must have type " + type.getName() + ".  "
                "It has type: " + node->getType().getName());
        }
    }


#define REN_DYNAMIC_CAST(name, type, object)    \
    type name = dynamic_cast<type>(object)


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
        } else if (REN_DYNAMIC_CAST(p, InfixNode*, node.get())) {
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


    CodeNodePtr evaluate(ConcreteNodePtr c) {
        if (REN_DYNAMIC_CAST(v, ValueNode*, c.get())) {
            return CodeNodePtr(new NameCodeNode(v->evaluate()));
        } else if (REN_DYNAMIC_CAST(a, ApplicationNode*, c.get())) {

            ConcreteNodePtr function = a->getFunction();
            const ConcreteNodeList& arguments = a->getArguments();

            if (REN_DYNAMIC_CAST(infix, InfixNode*, function.get())) {

                assert(arguments.size() == 2);
                CodeNodeList args(2);
                args[0] = evaluate(arguments[0]);
                args[1] = evaluate(arguments[1]);

                return CodeNodePtr(new CallCodeNode(
                                       true,
                                       infix->getOperator(),
                                       args));
            } else if (REN_DYNAMIC_CAST(ab, AbstractionNode*, function.get())) {

                ConcreteNodePtr inside = ab->getInside();
                const ConcreteNodeList& replacements = ab->getReplacements();

                assert(arguments.size() == replacements.size());

                ReplacementMap rm;
                for (size_t i = 0; i < arguments.size(); ++i) {
                    rm[replacements[i]] = arguments[i];
                }

                // clone inside, replacing arguments
                return evaluate(copyAndReplace(inside, rm));

            } else {
                assert(!"Error Unknown Function Type!");
                return CodeNodePtr();
            }

        } else if (REN_DYNAMIC_CAST(p, ArgumentNode*, c.get())) {
            assert(!"Can't directly evaluate an ArgumentNode");
            (void)p;
            return CodeNodePtr();
        } else if (REN_DYNAMIC_CAST(p, InfixNode*, c.get())) {
            assert(!"Can't directly evaluate an InfixNode");
            (void)p;
            return CodeNodePtr();
        } else {
            std::cout << typeid(c.get()).name() << std::endl;
            assert(!"Error Unknown ConcreteNode!");
            return CodeNodePtr();
        }
    }


    static CompileResult doCompile(
        const string& source,
        std::ostream& output
    ) {
        ProgramPtr program = parse(source);
        if (!program) {
            // No exceptions thrown, but no program generated.  Must be empty.
            return CompileResult(true);
        }

        CompilationContext cc(program);

        std::ostringstream vertexShader;
        std::ostringstream fragmentShader;

        if (program->hasDefinition("gl_Position")) {
            ConcreteNodePtr gl_Position = cc.instantiate("gl_Position");
            if (gl_Position) {
                requireType("gl_Position", gl_Position, VEC4);

                vertexShader
                    << "void main()\n"
                    << "{\n"
                    << "  gl_Position = "
                    << evaluate(gl_Position)->asExpression()
                    << ";\n"
                    << "}\n";
            }
        }

        if (program->hasDefinition("gl_FragColor")) {
            ConcreteNodePtr gl_FragColor = cc.instantiate("gl_FragColor");
            if (gl_FragColor) {
                requireType("gl_FragColor", gl_FragColor, VEC4);

                fragmentShader
                    << "void main()\n"
                    << "{\n"
                    << "  gl_FragColor = "
                    << evaluate(gl_FragColor)->asExpression()
                    << ";\n"
                    << "}\n";
            }
        }

        return CompileResult(
            true,
            vertexShader.str(),
            fragmentShader.str());
    }


    CompileResult compile(const string& source, std::ostream& output) {
        static CompileResult FAILURE(false);

        try {
            return doCompile(source, output);
        }
        catch (const antlr::ANTLRException& e) {
            output << "ANTLR Exception: " << e.toString() << std::endl;
            return FAILURE;
        }
        catch (const std::exception& e) {
            output << "Exception: " << e.what() << std::endl;
            return FAILURE;
        }

    }

}
