#ifndef REN_NODE_H
#define REN_NODE_H


#include <vector>
#include "Types.h"


namespace ren {

    class Node {
    public:
        virtual ~Node() { }
        virtual string toString() const = 0;
        virtual Type getType() const = 0;
        virtual string evaluate() const = 0;
    };
    typedef boost::shared_ptr<Node> NodePtr;
    typedef std::vector<NodePtr> NodeList;


    struct BuiltIn {
        string name;
        Type type;
    };

    static BuiltIn builtIns[] = {
        // Default attributes.
        { "gl_Color",          "vec4"   },
        { "gl_SecondaryColor", "vec4"   },
        { "gl_Normal",         "vec3"   },
        { "gl_Vertex",         "vec4"   },
        { "gl_FogCoord",       "float"  },
        { "gl_MultiTexCoord",  "vec4[]" },

        // Default varyings.
        
    };


#define ARRAY_SIZE(array) (sizeof(array) / sizeof(*(array)))

            
    class ApplicationNode : public Node {
    public:
        ApplicationNode(const string& name_, const NodeList& children_)
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
                return "<unknown>";
            }
        }

        string evaluate() const {
            return name;
        }

        static Type typeOf(const string& name) {
            if (isInteger(name)) return "int";
            if (isBuiltIn(name)) return getBuiltInType(name);
            return "";
        }

        static bool isInteger(const string& name) {
            for (size_t i = 0; i < name.size(); ++i) {
                if (!isdigit(name[i])) {
                    return false;
                }
            }
            return true;
        }

        static bool isBuiltIn(const string& name) {
            for (size_t i = 0; i < ARRAY_SIZE(builtIns); ++i) {
                if (builtIns[i].name == name) {
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
            // assert?  throw?
            return "<unknown>";
        }

        string name;
        NodeList children;
    };


    inline NodePtr makeBinaryNode(const char* name, NodePtr lhs, NodePtr rhs) {
        assert(lhs);
        assert(rhs);

        NodeList children(2);
        children[0] = lhs;
        children[1] = rhs;
        return NodePtr(new ApplicationNode(name, children));
    }

}

#endif
