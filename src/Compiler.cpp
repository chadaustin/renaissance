#include <sstream>
#include "ShaderLexer.hpp"
#include "ShaderParser.hpp"
#include "ShaderValidator.hpp"
#include "Compiler.h"


ProgramPtr validate(const string& source) {
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
            std::cerr << "Parser built no AST!" << std::endl;
        }
    }
    catch (const antlr::ANTLRException& e) {
        std::cerr << "ANTLR Exception: " << e.toString() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return ProgramPtr();
}


CompilerResult compile(const string& source) {
    static CompilerResult FAILURE;

    ProgramPtr program = validate(source);
    if (!program) {
        return CompilerResult();
    }

    std::ostringstream vertexShader;
    std::ostringstream fragmentShader;

    DefinitionPtr gl_Position = program->getDefinition("gl_Position");
    if (gl_Position) {
        if (gl_Position->type != "vec4") {
            std::cout << "gl_Position must have type vec4." << std::endl;
            return FAILURE;
        }
        if (gl_Position->arguments.size() != 0) {
            std::cout << "gl_Position must not take any arguments." << std::endl;
            return FAILURE;
        }
        vertexShader << "void main() {" << std::endl;
        vertexShader << "  gl_Position = something;" << std::endl;
        vertexShader << "}" << std::endl;
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
