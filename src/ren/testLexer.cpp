#include <iostream>
#include <fstream>
#include <ren/ShaderLexer.hpp>
using namespace ren;


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
        bool first = true;
        while (t->getType() != antlr::Token::EOF_TYPE) {
            if (t->getType() == ShaderLexerTokenTypes::NEWLINE) {
                endl(std::cout);
                first = true;
            } else {
                if (!first) {
                    std::cout << ' ';
                }
                std::cout << t->getText();

                first = false;
            }

            t = l.nextToken();
        }
    }
    catch (const antlr::ANTLRException& e) {
        std::cerr << "ANTLR Exception: " << e.toString() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

}
