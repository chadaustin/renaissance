#include <algorithm>
#include <iostream>
#include "Program.h"


void Program::print() {
    for (size_t i = 0; i < definitions.size(); ++i) {
        definitions[i]->print();
    }
}
