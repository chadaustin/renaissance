#ifndef REN_NODE_H
#define REN_NODE_H


#include <stdexcept>
#include <vector>
#include <ctype.h>
#include "Types.h"


namespace ren {

    struct BuiltIn {
        string name;
        Type type;
        bool function;
    };

    static BuiltIn builtIns[] = {
        // Default attributes.
        { "gl_Color",          VEC4   },
        { "gl_SecondaryColor", VEC4   },
        { "gl_Normal",         VEC3   },
        { "gl_Vertex",         VEC4   },
        { "gl_FogCoord",       FLOAT  },
        //{ "gl_MultiTexCoord",  "vec4[]" },

        // ftransform is kind of a special attribute in that it's a function call...
        { "ftransform",        VEC4, true },

        // Default varyings.

        // Default uniforms.

        // Built-in state.
        { "gl_ModelViewProjectionMatrix", MAT4 },
    };


#define ARRAY_SIZE(array) (sizeof(array) / sizeof(*(array)))

    class SyntaxNode;
    typedef boost::shared_ptr<SyntaxNode> SyntaxNodePtr;
    typedef std::vector<SyntaxNodePtr> SyntaxNodeList;

    class SyntaxNode {
    public:
        SyntaxNode(const string& name_, const SyntaxNodeList& children_)
        : name(name_)
        , children(children_) {
        }

        string toString() const {
            if (children.empty()) {
                return name;
            } else {
                string rv = "(" + name;
                for (size_t i = 0; i < children.size(); ++i) {
                    rv += " " + children[i]->toString();
                }
                return rv + ")";
            }
        }

        Type getType() const {
            if (children.empty()) {
                return typeOf(name);
            } else {
                if (name == "*") {
                    assert(children.size() == 2);
                    if (children[0]->getType() == MAT4 &&
                        children[1]->getType() == VEC4
                    ) {
                        return VEC4;
                    }
                }
                if (name == "+") {
                    assert(children.size() == 2);
                    SyntaxNodePtr lhs(children[0]);
                    SyntaxNodePtr rhs(children[1]);
                    if (lhs->getType() == rhs->getType()) {
                        return lhs->getType();
                    }
                }
                if (name == "++") {
                    return VEC2;
                }
                if (name == ".") {
                    return VEC4;
                }
                throw std::runtime_error("Unknown type");
            }
        }

        static string paren(const string& str) {
            return "(" + str + ")";
        }

        string evaluate() const {
            if (isFunction(name)) {
                return name + "()";
            } else {
                if (name == "*") {
                    assert(children.size() == 2);
                    SyntaxNodePtr lhs(children[0]);
                    SyntaxNodePtr rhs(children[1]);
                    return paren(lhs->evaluate()) + " " + name + " " +
                           paren(rhs->evaluate());
                } else {
                    return name;
                }
            }
        }

        static Type typeOf(const string& name) {
            if (isInteger(name)) return INT;
            if (isFloat(name))   return FLOAT;
            if (name == "true")  return BOOL;
            if (name == "false") return BOOL;
            if (isBuiltIn(name)) return getBuiltInType(name);
            return VEC4;
        }

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

        static bool isBuiltIn(const string& name) {
            for (size_t i = 0; i < ARRAY_SIZE(builtIns); ++i) {
                if (builtIns[i].name == name) {
                    return true;
                }
            }
            return false;
        }

        static bool isFunction(const string& name) {
            for (size_t i = 0; i < ARRAY_SIZE(builtIns); ++i) {
                if (builtIns[i].name == name && builtIns[i].function) {
                    return true;
                }
            }
            return false;
        }

        static Type getBuiltInType(const string& name) {            
            for (size_t i = 0; i < ARRAY_SIZE(builtIns); ++i) {
                if (builtIns[i].name == name) {
                    return builtIns[i].type;
                }
            }
            throw std::runtime_error("Unknown built-in type!");
        }

        string name;
        SyntaxNodeList children;
    };


    inline SyntaxNodePtr makeBinaryNode(const char* name, SyntaxNodePtr lhs, SyntaxNodePtr rhs) {
        assert(lhs);
        assert(rhs);

        SyntaxNodeList children(2);
        children[0] = lhs;
        children[1] = rhs;
        return SyntaxNodePtr(new SyntaxNode(name, children));
    }

}

#endif
