#include <iostream>
#include "Definition.h"


namespace ren {

    std::ostream& operator<<(std::ostream& os, const ArgumentList& list) {
        for (ArgumentList::const_iterator i = list.begin();
             i != list.end();
             ++i
        ) {
            if (i != list.begin()) {
                os << ' ';
            }
            os << *i;
        }
        return os;
    }

    void Definition::print() {
        std::cout << "Function: " << name << std::endl;
        if (!arguments.empty()) {
            std::cout << "  Arguments:";
            for (size_t i = 0; i < arguments.size(); ++i) {
                std::cout << " " << arguments[i];
            }
            std::cout << std::endl;
        }
        std::cout << "  Expression: " << expression->toString() << std::endl;
    }

}
