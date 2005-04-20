#include "Types.h"


namespace ren {

#define REN_NAME_CASE(type, name) case _##type: return (name)

    const string PrimitiveType::getName() const {
        switch (_code) {
            REN_NAME_CASE(FLOAT, "float");
            REN_NAME_CASE(VEC2,  "vec2");
            REN_NAME_CASE(VEC3,  "vec3");
            REN_NAME_CASE(VEC4,  "vec4");

            REN_NAME_CASE(INT,   "int");
            REN_NAME_CASE(VEC2I, "vec2i");
            REN_NAME_CASE(VEC3I, "vec3i");
            REN_NAME_CASE(VEC4I, "vec4i");

            REN_NAME_CASE(BOOL,  "bool");
            REN_NAME_CASE(VEC2B, "vec2b");
            REN_NAME_CASE(VEC3B, "vec3b");
            REN_NAME_CASE(VEC4B, "vec4b");

            REN_NAME_CASE(MAT2, "mat2");
            REN_NAME_CASE(MAT3, "mat3");
            REN_NAME_CASE(MAT4, "mat4");

            REN_NAME_CASE(SAMPLER1D,       "sampler1D");
            REN_NAME_CASE(SAMPLER2D,       "sampler2D");
            REN_NAME_CASE(SAMPLER3D,       "sampler3D");
            REN_NAME_CASE(SAMPLERCUBE,     "samplerCube");
            REN_NAME_CASE(SAMPLER1DSHADOW, "sampler1DShadow");
            REN_NAME_CASE(SAMPLER2DSHADOW, "sampler2DShadow");
            default: assert(false); return "<unknown>";
        }
    }

    bool PrimitiveType::operator==(const TypeObject& rhs) const {
        if (const PrimitiveType* p = dynamic_cast<const PrimitiveType*>(&rhs)) {
            return _code == p->_code;
        } else {
            return false;
        }
    }


    const string TupleType::getName() const {
        string result = "(";
        for (size_t i = 0; i < _elements.size(); ++i) {
            if (i != 0) {
                result += ", ";
            }
            result += _elements[i]->getName();
        }
        result += ")";
        return result;
    }

    bool TupleType::operator==(const TypeObject& rhs) const {
        if (const TupleType* p = dynamic_cast<const TupleType*>(&rhs)) {
            if (_elements.size() != p->_elements.size()) {
                return false;
            }
            for (size_t i = 0; i < _elements.size(); ++i) {
                if (*(_elements[i]) != *(p->_elements[i])) {
                    return false;
                }
            }
            return true;
        } else {
            return false;
        }
    }


    const string FunctionType::getName() const {
        return "(" + _in->getName() + " -> " + _out->getName() + ")";
    }

    bool FunctionType::operator==(const TypeObject& rhs) const {
        if (const FunctionType* p = dynamic_cast<const FunctionType*>(&rhs)) {
            return *_in == *p->_in && *_out == *p->_out;
        } else {
            return false;
        }
    }

}
