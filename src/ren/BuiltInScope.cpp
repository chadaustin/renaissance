#include "BuiltInScope.h"


namespace ren {

    static bool isInteger(const string& name) {
        for (size_t i = 0; i < name.size(); ++i) {
            if (!isdigit(name[i])) {
                return false;
            }
        }
        return true;
    }


    static bool isFloat(const string& name) {
        bool has_period = false;
        for (size_t i = 0; i < name.size(); ++i) {
            if (name[i] == '.') {
                if (has_period) {
                    return false;
                }
                has_period = true;
            } else if (!isdigit(name[i])) {
                return false;
            }
        }
        return has_period;
    }


    ConcreteNodePtr BuiltInScope::lookup(const string& name, Type argTypes) {
        if (name == "true" || name == "false") {
            if (Type(argTypes) == NullType) {
                return ConcreteNodePtr(new ValueNode(name, BOOL));
            } else {
                throw CompileError("Can't call a boolean.");
            }
        }

        if (isInteger(name)) {
            if (Type(argTypes) == NullType) {
                return ConcreteNodePtr(new ValueNode(name, INT));
            } else {
                throw CompileError("Can't call an integer.");
            }
        }

        if (isFloat(name)) {
            if (Type(argTypes) == NullType) {
                return ConcreteNodePtr(new ValueNode(name, FLOAT));
            } else {
                throw CompileError("Can't call a float.");
            }
        }

        enum BuiltInType {
            VALUE,
            NULLARY_FUNCTION,
            FUNCTION,
            INFIX,
            SWIZZLE,
        };

        struct BuiltIn {
            string name;
            Type type;
            BuiltInType nodeType;
        };

        static const BuiltIn builtIns[] = {
            { "*",          MAT4 * VEC4 >> VEC4,    INFIX },
            { "+",          INT * INT >> INT,       INFIX },
            { "/",          VEC2 * VEC2 >> VEC2,    INFIX },
            { "ftransform", VEC4,                   NULLARY_FUNCTION },
            { "gl_Vertex",  VEC4,                   VALUE },
            { "gl_Color",   VEC4,                   VALUE },
            { "gl_ModelViewProjectionMatrix", MAT4, VALUE },
            { "xyz", VEC4 >> VEC3,                  SWIZZLE },
            { "xy",  VEC4 >> VEC2,                  SWIZZLE },
            { "x",   VEC4 >> FLOAT,                 SWIZZLE },
            { "vec4", FLOAT * FLOAT * FLOAT * FLOAT >> VEC4, FUNCTION },
        };

#define REN_ARRAY_SIZE(array) (sizeof(array) / sizeof(*(array)))

        for (size_t i = 0; i < REN_ARRAY_SIZE(builtIns); ++i) {
            const BuiltIn& b = builtIns[i];
            if (name == b.name && argTypes == asFunction(b.type).in) {
                switch (b.nodeType) {
                    case VALUE:
                        return ConcreteNodePtr(
                            new ValueNode(b.name, b.type));
                        break;
                    case NULLARY_FUNCTION:
                        return ConcreteNodePtr(
                            new ValueNode(b.name, b.type,
                                          ValueNode::BUILTIN, true));
                        break;
                    case FUNCTION:
                        return ConcreteNodePtr(
                            new FunctionNode(b.name, b.type, FunctionNode::FUNCTION));
                        break;
                    case INFIX:
                        return ConcreteNodePtr(
                            new FunctionNode(b.name, b.type, FunctionNode::INFIX));
                        break;
                    case SWIZZLE:
                        return ConcreteNodePtr(
                            new FunctionNode(b.name, b.type, FunctionNode::SWIZZLE));
                        break;
                    default:
                        assert(!"Unknown Built-In Node Type");
                        break;
                }
            }
        }

        return ConcreteNodePtr();
    }

}
