#include <iostream>
#include <fstream>
#include "ShaderLexer.hpp"


int usage() {
    std::cerr << "Usage: testLexer filename" << std::endl;
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
        ShaderLexer l(shader);
        antlr::RefToken t = l.nextToken();
        while (t->getType() != antlr::Token::EOF_TYPE) {
            std::cout << t->getText() << std::endl;
            t = l.nextToken();
        }
    }
    catch (const antlr::ANTLRException& e) {
        std::cout << "ANTLR Exception: " << e.toString() << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }

}
