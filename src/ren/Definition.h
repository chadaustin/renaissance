#ifndef REN_DEFINITION_H
#define REN_DEFINITION_H


#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "Types.h"


namespace ren {

    struct Type { };
    struct Frequency { };

    struct Argument {
        string name;
    };
    typedef std::vector<Argument> ArgumentList;


    class Node {
    public:
        virtual ~Node() { }
        virtual string toString() const = 0;
    };
    typedef boost::shared_ptr<Node> NodePtr;


    class ApplicationNode : public Node {
    public:
        ApplicationNode(const string& name_, const std::vector<NodePtr>& children_)
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

        string name;
        std::vector<NodePtr> children;
    };


    class BinaryNode : public Node {
    public:
        BinaryNode(const char* name_, NodePtr lhs_, NodePtr rhs_)
        : name(name_)
        , lhs(lhs_)
        , rhs(rhs_) {
        }

        string toString() const {
            return "(" + name
                + " " + lhs->toString()
                + " " + rhs->toString() + ")";
        }
        
        string name;  // function
        NodePtr lhs;
        NodePtr rhs;
    };


    struct Definition {
        void print();
        
        string name;
        string type;
        //Frequency frequency;
        ArgumentList arguments;
        NodePtr expression;
    };
    typedef boost::shared_ptr<Definition> DefinitionPtr;

}


#endif
