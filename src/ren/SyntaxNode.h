#ifndef REN_NODE_H
#define REN_NODE_H


#include <stdexcept>
#include <vector>
#include <ctype.h>
#include "Types.h"


namespace ren {

    class SyntaxNode;
    typedef boost::shared_ptr<SyntaxNode> SyntaxNodePtr;
    typedef std::vector<SyntaxNodePtr> SyntaxNodeList;


    class SyntaxNode {
    public:
        virtual ~SyntaxNode() { }
        virtual string toString() const = 0;
    };


    class ApplySyntaxNode : public SyntaxNode {
    public:
        ApplySyntaxNode(const SyntaxNodeList& children)
        : _children(children) {
            assert(children.size() >= 2);
        }

        string toString() const {
            string rv = "(";
            for (size_t i = 0; i < _children.size(); ++i) {
                if (i != 0) {
                    rv += " ";
                }
                rv += _children[i]->toString();
            }
            return rv + ")";
        }

        const SyntaxNodeList& getChildren() const {
            return _children;
        }

    private:
        SyntaxNodeList _children;
    };
    typedef boost::shared_ptr<ApplySyntaxNode> ApplySyntaxNodePtr;


    class ValueSyntaxNode : public SyntaxNode {
    public:
        ValueSyntaxNode(const string& name)
        : _name(name) {
        }

        string toString() const {
            return _name;
        }

        string getName() const {
            return _name;
        }

    private:
        string _name;
    };
    typedef boost::shared_ptr<ValueSyntaxNode> ValueSyntaxNodePtr;


    inline SyntaxNodePtr makeUnaryNode(
        const string& name,
        SyntaxNodePtr lhs
    ) {
        assert(lhs);

        SyntaxNodeList children(2);
        children[0].reset(new ValueSyntaxNode(name));
        children[1] = lhs;
        return SyntaxNodePtr(new ApplySyntaxNode(children));
    }


    inline SyntaxNodePtr makeBinaryNode(
        const string& name,
        SyntaxNodePtr lhs,
        SyntaxNodePtr rhs
    ) {
        assert(lhs);
        assert(rhs);

        SyntaxNodeList children(3);
        children[0].reset(new ValueSyntaxNode(name));
        children[1] = lhs;
        children[2] = rhs;
        return SyntaxNodePtr(new ApplySyntaxNode(children));
    }

}

#endif
