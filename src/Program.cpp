#include <algorithm>
#include <iostream>
#include "Program.h"


void Program::print() {
    std::for_each(definitions.begin(), definitions.end(),
                  std::mem_fun_ref(&Definition::print));
}
