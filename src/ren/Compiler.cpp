#include <sstream>
#include "ShaderLexer.hpp"
#include "ShaderParser.hpp"
#include "ShaderValidator.hpp"
#include "Compiler.h"


namespace ren {

    ProgramPtr parse(const string& source, std::ostream& output) {
        try {
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
        catch (const antlr::ANTLRException& e) {
            output << "ANTLR Exception: " << e.toString() << std::endl;
        }
        catch (const std::exception& e) {
            output << "Exception: " << e.what() << std::endl;
        }

        return ProgramPtr();
    }

    ProgramPtr analyze(const string& source, std::ostream& output) {
        ProgramPtr program = parse(source, output);
        if (program) {
            program->inferTypes();
        }
        return program;
    }


    CompilerResult compile(const string& source, std::ostream& output) {
        static CompilerResult FAILURE;

        ProgramPtr program = analyze(source, output);
        if (!program) {
            return CompilerResult();
        }

        std::ostringstream vertexShader;
        std::ostringstream fragmentShader;

        DefinitionPtr gl_Position = program->getDefinition("gl_Position");
        if (gl_Position) {
            if (gl_Position->type != "vec4") {
                output << "gl_Position must have type vec4.  It has type: "
                       << gl_Position->type << std::endl;
                return FAILURE;
            }
            if (gl_Position->arguments.size() != 0) {
                output << "gl_Position must not take any arguments."
                       << std::endl;
                return FAILURE;
            }
            vertexShader << "void main()\n"
                         << "{\n"
                         << "  gl_Position = "
                         << gl_Position->expression->evaluate()
                         << ";\n"
                         << "}\n";
        }

        DefinitionPtr gl_FragColor = program->getDefinition("gl_FragColor");
        if (gl_FragColor) {
            fragmentShader << "void main() {" << std::endl;
            fragmentShader << "  gl_FragColor = something;" << std::endl;
            fragmentShader << "}" << std::endl;
        }

        CompilerResult cr;
        cr.vertexShader = vertexShader.str();
        cr.fragmentShader = fragmentShader.str();
        return cr;
    }

}
