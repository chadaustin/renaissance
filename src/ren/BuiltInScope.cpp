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
        // Boolean constants.
        if (name == "true" || name == "false") {
            if (Type(argTypes) == NullType) {
                return ConcreteNodePtr(new ValueNode(name, BOOL));
            } else {
                throw CompileError("Can't call a boolean.");
            }
        }

        // Integer constants.
        if (isInteger(name)) {
            if (Type(argTypes) == NullType) {
                return ConcreteNodePtr(new ValueNode(name, INT));
            } else {
                throw CompileError("Can't call an integer.");
            }
        }

        // Float constants.
        if (isFloat(name)) {
            if (Type(argTypes) == NullType) {
                return ConcreteNodePtr(new ValueNode(name, FLOAT));
            } else {
                throw CompileError("Can't call a float.");
            }
        }

        // If construct.
        if (name == "if") {
            TypeList tl(asTuple(argTypes));
            if (tl.size() != 3) {
                throw CompileError("if construct requires three arguments.");
            }
            if (tl[0] != BOOL) {
                throw CompileError("if condition must have type bool.");
            }
            if (tl[1] != tl[2]) {
                throw CompileError("if true-part and false-part must have same type.");
            }
            return ConcreteNodePtr(new IfNode(argTypes >> tl[1]));
        }

        enum BuiltInType {
            VALUE,
            NULLARY_FUNCTION,
            FUNCTION,
            INFIX,
            PREFIX,
            SWIZZLE,
        };

        struct BuiltIn {
            string name;
            Type type;
            BuiltInType nodeType;
        };

        static const BuiltIn builtIns[] = {
            { "*",          MAT4 * VEC4 >> VEC4,    INFIX },
            { "*", MAT3 * VEC3 >> VEC3, INFIX },
            { "*",          FLOAT * FLOAT >> FLOAT, INFIX },

            { "+",          INT * INT >> INT,       INFIX },
            { "+",          VEC2 * VEC2 >> VEC2,    INFIX },
            { "+", FLOAT * FLOAT >> FLOAT, INFIX },
            { "/",          VEC2 * VEC2 >> VEC2,    INFIX },
            { "/",          FLOAT * FLOAT >> FLOAT, INFIX },

            { "-",          FLOAT * FLOAT >> FLOAT, INFIX },
            { "-",          VEC3 * VEC3 >> VEC3,    INFIX },

            { "-", VEC3 >> VEC3, PREFIX },

            { ">",          FLOAT * FLOAT >> BOOL,  INFIX },

            { "ftransform", VEC4,                   NULLARY_FUNCTION },
            { "gl_Vertex",  VEC4,                   VALUE },
            { "gl_Color",   VEC4,                   VALUE },
            { "gl_Normal",  VEC3, VALUE },

            { "gl_ModelViewMatrix",           MAT4, VALUE },
            { "gl_NormalMatrix",              MAT3, VALUE },
            { "gl_ModelViewProjectionMatrix", MAT4, VALUE },

            { "normalize", VEC3 >> VEC3, FUNCTION },

            { "reflect", VEC3 * VEC3 >> VEC3, FUNCTION },

            { "pow", FLOAT * FLOAT >> FLOAT, FUNCTION },

            { "mix",   VEC3 * VEC3 * FLOAT           >> VEC3, FUNCTION },
            { "vec2",  FLOAT * FLOAT                 >> VEC2, FUNCTION },
            { "vec4",  FLOAT * FLOAT * FLOAT * FLOAT >> VEC4, FUNCTION },

            { "fract", FLOAT >> FLOAT,                        FUNCTION },
            { "fract", VEC2  >> VEC2,                         FUNCTION },

            { "step",  VEC2 * VEC2 >> VEC2, FUNCTION },

            { "dot", VEC3 * VEC3 >> FLOAT, FUNCTION },

            { "max", FLOAT * FLOAT >> FLOAT, FUNCTION },

            { "y",   VEC2 >> FLOAT,                 SWIZZLE },
            { "xyz", VEC4 >> VEC3,                  SWIZZLE },
            { "xy",  VEC4 >> VEC2,                  SWIZZLE },
            { "x",   VEC4 >> FLOAT,                 SWIZZLE },
            { "x",   VEC2 >> FLOAT,                 SWIZZLE },
            { "wxyz", VEC4 >> VEC4,                 SWIZZLE },

            { "++",  VEC3 * FLOAT >> VEC4,          FUNCTION },
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
                    case PREFIX:
                        return ConcreteNodePtr(
                            new FunctionNode(b.name, b.type, FunctionNode::PREFIX));
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
