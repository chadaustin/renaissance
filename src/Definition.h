#ifndef DEFINITION_H
#define DEFINITION_H


#include <string>
#include <vector>


struct Type { };
struct Frequency { };
struct Argument { };
struct Expression { };


struct Definition {
    void print();

    std::string name;
    Type type;
    Frequency frequency;
    std::vector<Argument> arguments;
    Expression expression;
};


#endif
