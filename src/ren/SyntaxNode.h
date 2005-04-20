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

        static string paren(const string& str) {
            return "(" + str + ")";
        }

        string name;
        SyntaxNodeList children;
    };


    inline SyntaxNodePtr makeBinaryNode(
        const string& name,
        SyntaxNodePtr lhs,
        SyntaxNodePtr rhs
    ) {
        assert(lhs);
        assert(rhs);

        SyntaxNodeList children(2);
        children[0] = lhs;
        children[1] = rhs;
        return SyntaxNodePtr(new SyntaxNode(name, children));
    }

}

#endif
