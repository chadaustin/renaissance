#include <sstream>
#include "ShaderLexer.hpp"
#include "ShaderParser.hpp"
#include "ShaderValidator.hpp"
#include "Compiler.h"


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


    CompileResult compile(const string& source, std::ostream& output) {
        static CompileResult FAILURE(false);

        ProgramPtr program;
        try {
            program = parse(source);
            if (!program) {
                return CompileResult(true);
            }
        }
        catch (const antlr::ANTLRException& e) {
            output << "ANTLR Exception: " << e.toString() << std::endl;
            return FAILURE;
        }
        catch (const std::exception& e) {
            output << "Exception: " << e.what() << std::endl;
            return FAILURE;
        }

        std::ostringstream vertexShader;

        InstancePtr gl_Position = program->resolveDefinition("gl_Position");
        if (gl_Position) {
            if (gl_Position->type != VEC4) {
                output << "gl_Position must have type vec4.  It has type: "
                       << gl_Position->type << std::endl;
                return FAILURE;
            }
            vertexShader << "void main()\n"
                         << "{\n"
                         << "  gl_Position = "
                         << gl_Position->expression->evaluate()
                         << ";\n"
                         << "}\n";
        }

        std::ostringstream fragmentShader;

        DefinitionPtr gl_FragColor = program->getDefinition("gl_FragColor");
        if (gl_FragColor) {
            if (gl_FragColor->type != VEC4) {
                output << "gl_FragColor must have type vec4.  It has type: "
                       << gl_FragColor->type << std::endl;
                return FAILURE;
            }
            if (gl_FragColor->arguments.size() != 0) {
                output << "gl_FragColor must not take any arguments."
                       << std::endl;
                return FAILURE;
            }
            fragmentShader << "void main()\n"
                           << "{\n"
                           << "  gl_FragColor = "
                           << gl_FragColor->expression->evaluate()
                           << ";\n"
                           << "}\n";
        }

        return CompileResult(true, vertexShader.str(), fragmentShader.str());
    }

}
