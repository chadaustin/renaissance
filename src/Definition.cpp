#include <iostream>
#include "Definition.h"


void Definition::print() {
    std::cout << "Function: " << name << std::endl;
    if (!arguments.empty()) {
        std::cout << "  Arguments:";
        for (size_t i = 0; i < arguments.size(); ++i) {
            std::cout << " " << arguments[i].name;
        }
        std::cout << std::endl;
    }
    std::cout << "  Expression: " << expression->toString() << std::endl;
}
