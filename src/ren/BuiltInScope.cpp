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
                return ConcreteNodePtr(new ValueNode(name, BOOL, CONSTANT));
            } else {
                throw CompileError("Can't call a boolean.");
            }
        }

        // Integer constants.
        if (isInteger(name)) {
            if (Type(argTypes) == NullType) {
                return ConcreteNodePtr(new ValueNode(name, INT, CONSTANT));
            } else {
                throw CompileError("Can't call an integer.");
            }
        }

        // Float constants.
        if (isFloat(name)) {
            if (Type(argTypes) == NullType) {
                return ConcreteNodePtr(new ValueNode(name, FLOAT, CONSTANT));
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
                    return ConcreteNodePtr(
                        new FunctionNode(vec.getName(), el1 * el2 >> vec,
                                         FunctionNode::FUNCTION));
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
        };

        static const BuiltIn builtIns[] = {
            { "*", MAT4 * VEC4 >> VEC4,    CONSTANT, INFIX },
            { "*", MAT3 * VEC3 >> VEC3,    CONSTANT, INFIX },
            { "*", FLOAT * FLOAT >> FLOAT, CONSTANT, INFIX },

            { "+", INT * INT >> INT,       CONSTANT, INFIX },
            { "+", VEC2 * VEC2 >> VEC2,    CONSTANT, INFIX },
            { "+", FLOAT * FLOAT >> FLOAT, CONSTANT, INFIX },
            { "+", VEC4 * VEC4 >> VEC4,    CONSTANT, INFIX },

            { "/", VEC2 * VEC2 >> VEC2,    CONSTANT, INFIX },
            { "/", FLOAT * FLOAT >> FLOAT, CONSTANT, INFIX },

            { "-", FLOAT * FLOAT >> FLOAT, CONSTANT, INFIX },
            { "-", VEC3 * VEC3 >> VEC3,    CONSTANT, INFIX },

            { "+", FLOAT >> FLOAT, CONSTANT, PREFIX },
            { "-", FLOAT >> FLOAT, CONSTANT, PREFIX },
            { "-", VEC3 >> VEC3,   CONSTANT, PREFIX },

            { ">", FLOAT * FLOAT >> BOOL, CONSTANT, INFIX },

            { "ftransform", VEC4, VERTEX, NULLARY_FUNCTION },
            { "gl_Vertex",  VEC4, VERTEX, VALUE },
            { "gl_Color",   VEC4, VERTEX, VALUE },
            { "gl_Normal",  VEC3, VERTEX, VALUE },

            { "gl_ModelViewMatrix",           MAT4, UNIFORM, VALUE },
            { "gl_NormalMatrix",              MAT3, UNIFORM, VALUE },
            { "gl_ModelViewProjectionMatrix", MAT4, UNIFORM, VALUE },

            { "normalize", VEC3 >> VEC3, CONSTANT, FUNCTION },

            { "reflect", VEC3 * VEC3 >> VEC3, CONSTANT, FUNCTION },

            { "pow", FLOAT * FLOAT >> FLOAT, CONSTANT, FUNCTION },

            { "mix",   VEC3 * VEC3 * FLOAT           >> VEC3, CONSTANT, FUNCTION },
            { "vec2",  FLOAT * FLOAT                 >> VEC2, CONSTANT, FUNCTION },
            { "vec4",  FLOAT * FLOAT * FLOAT * FLOAT >> VEC4, CONSTANT, FUNCTION },

            { "fract", FLOAT >> FLOAT, CONSTANT, FUNCTION },
            { "fract", VEC2  >> VEC2,  CONSTANT, FUNCTION },

            { "step",  VEC2 * VEC2 >> VEC2, CONSTANT, FUNCTION },

            { "dot", VEC3 * VEC3 >> FLOAT, CONSTANT, FUNCTION },

            { "max", FLOAT * FLOAT >> FLOAT, CONSTANT, FUNCTION },

            { "y",   VEC2 >> FLOAT, CONSTANT, SWIZZLE },
            { "y",   VEC4 >> FLOAT, CONSTANT, SWIZZLE },
            { "xyz", VEC4 >> VEC3,  CONSTANT, SWIZZLE },
            { "xy",  VEC4 >> VEC2,  CONSTANT, SWIZZLE },
            { "x",   VEC4 >> FLOAT, CONSTANT, SWIZZLE },
            { "x",   VEC2 >> FLOAT, CONSTANT, SWIZZLE },
            { "wxyz", VEC4 >> VEC4, CONSTANT, SWIZZLE },
        };

#define REN_ARRAY_SIZE(array) (sizeof(array) / sizeof(*(array)))

        for (size_t i = 0; i < REN_ARRAY_SIZE(builtIns); ++i) {
            const BuiltIn& b = builtIns[i];
            if (name == b.name && argTypes == asFunction(b.type).in) {
                switch (b.nodeType) {
                    case VALUE:
                        return ConcreteNodePtr(
                            new ValueNode(b.name, b.type, b.frequency));
                        break;
                    case NULLARY_FUNCTION:
                        return ConcreteNodePtr(
                            new ValueNode(b.name, b.type, b.frequency,
                                          ValueNode::BUILTIN, true));
                        break;
                    case FUNCTION:
                        assert(b.frequency == CONSTANT);
                        return ConcreteNodePtr(
                            new FunctionNode(b.name, b.type, FunctionNode::FUNCTION));
                        break;
                    case PREFIX:
                        assert(b.frequency == CONSTANT);
                        return ConcreteNodePtr(
                            new FunctionNode(b.name, b.type, FunctionNode::PREFIX));
                        break;
                    case INFIX:
                        assert(b.frequency == CONSTANT);
                        return ConcreteNodePtr(
                            new FunctionNode(b.name, b.type, FunctionNode::INFIX));
                        break;
                    case SWIZZLE:
                        assert(b.frequency == CONSTANT);
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
