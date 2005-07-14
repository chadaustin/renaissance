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


#define RETURN_PTR(ptr) return ConcreteNodePtr(ptr)


    ConcreteNodePtr BuiltInScope::lookup(const string& name, Type argTypes) {
        // Boolean constants.
        if (name == "true" || name == "false") {
            if (Type(argTypes) == NullType) {
                bool value = (name == "true");
                RETURN_PTR(
                    new ValueNode(
                        name,
                        BOOL,
                        CONSTANT,
                        Value::create(BOOL, &value)));
            } else {
                throw CompileError("Can't call a boolean.");
            }
        }

        // Integer constants.
        if (isInteger(name)) {
            if (Type(argTypes) == NullType) {
                int value = atoi(name.c_str());
                RETURN_PTR(
                    new ValueNode(
                        name,
                        INT,
                        CONSTANT,
                        Value::create(INT, &value)));
            } else {
                throw CompileError("Can't call an integer.");
            }
        }

        // Float constants.
        if (isFloat(name)) {
            if (Type(argTypes) == NullType) {
                float value = atof(name.c_str());
                RETURN_PTR(
                    new ValueNode(
                        name,
                        FLOAT,
                        CONSTANT,
                        Value::create(FLOAT, &value)));
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
            RETURN_PTR(new IfNode(argTypes >> tl[1]));
        }

        const Linearity PUNT = NONLINEAR;

        // Vector concatenation.
        if (name == "++") {
            TypeList tl(asTuple(argTypes));
            if (tl.size() == 2) {
                Type el1 = getElementType(tl[0]);
                Type el2 = getElementType(tl[1]);
                if (el1 == el2 && el1 != NullType) {
                    int length1 = getVectorLength(tl[0]);
                    int length2 = getVectorLength(tl[1]);
                    Type vec = getVectorType(el1, length1 + length2);
                    RETURN_PTR(
                        new FunctionNode(
                            vec.getName(), el1 * el2 >> vec,
                            FunctionNode::FUNCTION, PUNT));
                }
            }
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
            Frequency frequency;
            BuiltInType nodeType;
            Linearity linearity;
        };

        static const BuiltIn builtIns[] = {
            { "*", MAT4 * VEC4 >> VEC4,    CONSTANT, INFIX, PARTIALLY_LINEAR },
            { "*", MAT3 * VEC3 >> VEC3,    CONSTANT, INFIX, PARTIALLY_LINEAR },
            { "*", FLOAT * FLOAT >> FLOAT, CONSTANT, INFIX, PARTIALLY_LINEAR },
            { "*", VEC4 * VEC4 >> VEC4,    CONSTANT, INFIX, PARTIALLY_LINEAR },

            { "+", INT * INT >> INT,       CONSTANT, INFIX, LINEAR },
            { "+", VEC2 * VEC2 >> VEC2,    CONSTANT, INFIX, LINEAR },
            { "+", FLOAT * FLOAT >> FLOAT, CONSTANT, INFIX, LINEAR },
            { "+", VEC4 * VEC4 >> VEC4,    CONSTANT, INFIX, LINEAR },

            { "/", VEC2 * VEC2 >> VEC2,    CONSTANT, INFIX, NONLINEAR },
            { "/", FLOAT * FLOAT >> FLOAT, CONSTANT, INFIX, NONLINEAR },
            { "-", FLOAT * FLOAT >> FLOAT, CONSTANT, INFIX, LINEAR },
            { "-", VEC3 * VEC3 >> VEC3,    CONSTANT, INFIX, LINEAR },

            { "+", FLOAT >> FLOAT, CONSTANT, PREFIX, LINEAR },
            { "-", FLOAT >> FLOAT, CONSTANT, PREFIX, LINEAR },
            { "-", VEC3 >> VEC3,   CONSTANT, PREFIX, LINEAR },

            { ">", FLOAT * FLOAT >> BOOL, CONSTANT, INFIX, NONLINEAR },

            { "ftransform", VEC4, VERTEX, NULLARY_FUNCTION },
            { "gl_Vertex",  VEC4, VERTEX, VALUE },
            { "gl_Color",   VEC4, VERTEX, VALUE },
            { "gl_Normal",  VEC3, VERTEX, VALUE },

            { "gl_ModelViewMatrix",           MAT4, UNIFORM, VALUE },
            { "gl_NormalMatrix",              MAT3, UNIFORM, VALUE },
            { "gl_ModelViewProjectionMatrix", MAT4, UNIFORM, VALUE },

            { "gl_FragCoord",                 VEC2, FRAGMENT, VALUE },

            { "normalize", VEC3 >> VEC3, CONSTANT, FUNCTION, PUNT },

            { "reflect", VEC3 * VEC3 >> VEC3, CONSTANT, FUNCTION, PUNT },

            { "pow", FLOAT * FLOAT >> FLOAT, CONSTANT, FUNCTION, PUNT },

            { "mix",   VEC3 * VEC3 * FLOAT           >> VEC3, CONSTANT, FUNCTION, PUNT },
            { "vec2",  FLOAT * FLOAT                 >> VEC2, CONSTANT, FUNCTION, LINEAR },
            { "vec3",  FLOAT * FLOAT * FLOAT         >> VEC3, CONSTANT, FUNCTION, LINEAR },
            { "vec4",  FLOAT * FLOAT * FLOAT * FLOAT >> VEC4, CONSTANT, FUNCTION, LINEAR },

            { "fract", FLOAT >> FLOAT, CONSTANT, FUNCTION, PUNT },
            { "fract", VEC2  >> VEC2,  CONSTANT, FUNCTION, PUNT },

            { "step",  VEC2 * VEC2 >> VEC2, CONSTANT, FUNCTION, PUNT },

            { "dot", VEC3 * VEC3 >> FLOAT, CONSTANT, FUNCTION, PUNT },

            { "max", FLOAT * FLOAT >> FLOAT, CONSTANT, FUNCTION, PUNT },

            { "y",   VEC2 >> FLOAT, CONSTANT, SWIZZLE, LINEAR },
            { "y",   VEC4 >> FLOAT, CONSTANT, SWIZZLE, LINEAR },
            { "xyz", VEC4 >> VEC3,  CONSTANT, SWIZZLE, LINEAR },
            { "xy",  VEC4 >> VEC2,  CONSTANT, SWIZZLE, LINEAR },
            { "x",   VEC4 >> FLOAT, CONSTANT, SWIZZLE, LINEAR },
            { "x",   VEC2 >> FLOAT, CONSTANT, SWIZZLE, LINEAR },
            { "wxyz", VEC4 >> VEC4, CONSTANT, SWIZZLE, LINEAR },
        };

#define REN_ARRAY_SIZE(array) (sizeof(array) / sizeof(*(array)))

        for (size_t i = 0; i < REN_ARRAY_SIZE(builtIns); ++i) {
            const BuiltIn& b = builtIns[i];
            if (name == b.name && argTypes == asFunction(b.type).in) {
                switch (b.nodeType) {
                    case VALUE:
                        RETURN_PTR(
                            new ValueNode(
                                b.name, b.type, b.frequency, NullValue));
                        break;
                    case NULLARY_FUNCTION:
                        RETURN_PTR(
                            new ValueNode(
                                b.name, b.type, b.frequency,
                                NullValue, ValueNode::BUILTIN, true));
                        break;
                    case FUNCTION:
                        assert(b.frequency == CONSTANT);
                        RETURN_PTR(
                            new FunctionNode(
                                b.name, b.type,
                                FunctionNode::FUNCTION, b.linearity));
                        break;
                    case PREFIX:
                        assert(b.frequency == CONSTANT);
                        RETURN_PTR(
                            new FunctionNode(
                                b.name, b.type,
                                FunctionNode::PREFIX, b.linearity));
                        break;
                    case INFIX:
                        assert(b.frequency == CONSTANT);
                        RETURN_PTR(
                            new FunctionNode(
                                b.name, b.type,
                                FunctionNode::INFIX, b.linearity));
                        break;
                    case SWIZZLE:
                        assert(b.frequency == CONSTANT);
                        RETURN_PTR(
                            new FunctionNode(
                                b.name, b.type,
                                FunctionNode::SWIZZLE, b.linearity));
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
