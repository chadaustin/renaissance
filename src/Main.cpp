#include <fstream>
#include "ShaderLexer.hpp"
#include "ShaderParser.hpp"


int usage() {
    std::cerr << "Usage: parserTest filename" << std::endl;
    return -1;
}


int main(int argc, char** argv) {
    if (argc < 2) {
        return usage();
    }

    std::ifstream shader(argv[1]);
    if (!shader) {
        std::cerr << "File '" << argv[1] << "' does not exist." << std::endl;
        return usage();
    }

    try {
        ShaderLexer l(shader);
        ShaderParser p(l);
        p.program();
    }
    catch (const antlr::ANTLRException& e) {
        std::cout << "ANTLR Exception: " << e.toString() << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }
}
