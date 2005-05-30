#ifndef REN_INPUT_H
#define REN_INPUT_H


#include "Program.h"


namespace ren {

    class Bool {
    public:
        Bool(ProgramPtr program, const string& name)
        : _program(program)
        , _name(name) {
            const Constant* c = _program->getConstant(name);
            if (!c) {
                throw std::runtime_error("Program has no constant named " + name);
            }
            if (c->getType() != BOOL) {
                throw std::runtime_error("Program has no constant named " + name + " of type bool");
            }
        }

        Bool& operator=(bool value) {
            ValuePtr v = Value::create(BOOL, &value);
            _program->setConstantValue(_name, v);
            return *this;
        }

    private:
        ProgramPtr _program;
        string _name;
    };

}


#endif
