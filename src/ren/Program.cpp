#include <algorithm>
#include <iostream>
#include "Program.h"


namespace ren {

    void Program::print() {
        for (size_t i = 0; i < _uniforms.size(); ++i) {
            std::cout << "Uniform: " << _uniforms[i].getType()
                      << " " << _uniforms[i].getName() << std::endl;
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
