#include <iostream>
#include <fstream>
#include "ShaderLexer.hpp"
#include "ShaderParser.hpp"
#include "ShaderValidator.hpp"


int usage() {
    std::cerr << "Usage: testValidator filename" << std::endl;
    return -1;
}


int main(int argc, char** argv) {
    if (argc != 2) {
        return usage();
    }

    std::ifstream shader(argv[1]);
    if (!shader) {
        std::cerr << "File '" << argv[1] << "' does not exist." << std::endl;
        return usage();
    }

    try {
        antlr::ASTFactory parserFactory;
        ShaderLexer lexer(shader);
        ShaderParser parser(lexer);
        parser.initializeASTFactory(parserFactory);
        parser.setASTFactory(&parserFactory);
        parser.program();

        if (antlr::RefAST ast = parser.getAST()) {
            ShaderValidator validator;
            ProgramPtr program = validator.program(ast);

            program->print();
        } else {
            std::cerr << "Parser built no AST!" << std::endl;
        }
    }
    catch (const antlr::ANTLRException& e) {
        std::cout << "ANTLR Exception: " << e.toString() << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }
}
