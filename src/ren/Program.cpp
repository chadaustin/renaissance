#include <algorithm>
#include <iostream>
#include "Program.h"


namespace ren {

    void Program::print() {
        for (size_t i = 0; i < uniforms.size(); ++i) {
            std::cout << "Uniform: " << uniforms[i].getType()
                      << " " << uniforms[i].getName() << std::endl;
        }

        for (size_t i = 0; i < attributes.size(); ++i) {
            std::cout << "Attribute: " << attributes[i].getType()
                      << " " << attributes[i].getName() << std::endl;
        }

        for (size_t i = 0; i < definitions.size(); ++i) {
            definitions[i]->print();
        }
    }

}
