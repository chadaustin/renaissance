#ifndef PROGRAM_H
#define PROGRAM_H


#include <vector>
#include <boost/shared_ptr.hpp>
#include "Definition.h"


struct Program {
    void print();

    std::vector<Definition> definitions;
};
typedef boost::shared_ptr<Program> ProgramPtr;


#endif
