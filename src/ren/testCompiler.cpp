#include <iostream>
#include <vector>
#include <stdio.h>
#include <boost/shared_ptr.hpp>
#include "Compiler.h"
using namespace ren;


int usage() {
    std::cerr << "Usage: compiler filename" << std::endl;
    return -1;
}


int main(int argc, char** argv) {
    if (argc != 2) {
        return usage();
    }

    boost::shared_ptr<FILE> file(fopen(argv[1], "rb"), fclose);
    if (!file) {
        std::cerr << "File '" << argv[1] << "' cannot be read." << std::endl;
        return usage();
    }

    fseek(file.get(), 0, SEEK_END);
    long length = ftell(file.get());
    rewind(file.get());

    if (length < 0) {
        std::cerr << "Length of file is less than zero." << std::endl;
        return usage();
    }

    std::vector<char> contents(length + 1);
    if (fread(&contents[0], 1, length, file.get()) != size_t(length)) {
        std::cerr << "Couldn't read full contents of file." << std::endl;
        return usage();
    }
    contents[length] = 0;  // zero-terminate.

    CompileResult cr = compile(&contents[0]);
    if (cr.success) {
        if (!cr.vertexShader.empty()) {
            std::cout << "#### GLSL Vertex Shader:\n"
                      << cr.vertexShader
                      << std::endl;
        }
        if (!cr.fragmentShader.empty()) {
            std::cout << "#### GLSL Fragment Shader:\n"
                      << cr.fragmentShader
                      << std::endl;
        }
    } else {
        std::cout << "Compile failed." << std::endl;
    }
}
