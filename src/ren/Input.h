#ifndef REN_INPUT_H
#define REN_INPUT_H


#include "Program.h"


namespace ren {

    class InputVariable {
    protected:
        InputVariable(ProgramPtr program, const string& name, Type type)
        : _program(program)
        , _name(name) {
            const Input* c = getProgram()->getConstant(name);
            if (!c) {
                c = getProgram()->getUniform(name);
                _uniform = true;
            } else {
                _uniform = false;
            }
            checkType(name, c, type);
        }

        ProgramPtr getProgram() const {
            return _program;
        }

        void setValue(ValuePtr v) {
            if (_uniform) {
                getProgram()->setUniformValue(_name, v);
            } else {
                getProgram()->setConstantValue(_name, v);
            }
        }

        static void checkType(const string& name, const Input* i, Type type) {
            if (!i) {
                throw std::runtime_error("Program has no constant or uniform named " + name);
            }
            if (i->getType() != type) {
                throw std::runtime_error("Program constant or uniform named " + name + ", but it does not have type " + type.getName());
            }
        }

    private:
        ProgramPtr _program;
        string _name;
        bool _uniform;
    };


    class Bool : public InputVariable {
    public:
        Bool(ProgramPtr program, const string& name)
        : InputVariable(program, name, BOOL) {
        }

        Bool& operator=(bool value) {
            setValue(Value::create(BOOL, &value));
            return *this;
        }
    };


    class Vec3 : public InputVariable {
    public:
        Vec3(ProgramPtr program, const string& name)
        : InputVariable(program, name, VEC3) {
        }

        void set(float x, float y, float z) {
            const float values[3] = {x, y, z};
            setValue(Value::create(VEC3, values));
        }
    };

}


#endif
