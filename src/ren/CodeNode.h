#ifndef CODE_NODE_H
#define CODE_NODE_H


#include "Types.h"


namespace ren {

    class CodeNode {
    public:
        virtual ~CodeNode() { }
        virtual string asExpression() const = 0;
    };
    REN_SHARED_PTR(CodeNode);

    typedef std::vector<CodeNodePtr> CodeNodeList;


    class CallCodeNode : public CodeNode {
    public:
        CallCodeNode(
            bool infix,
            const string& op,
            const CodeNodeList& arguments)
        : _infix(infix)
        , _op(op)
        , _arguments(arguments) {
        }

        string asExpression() const {
            if (_infix) {
                assert(_arguments.size() == 2);
                string lhs = _arguments[0]->asExpression();
                string rhs = _arguments[1]->asExpression();
                return "(" + lhs + " " + _op + " " + rhs + ")";
            } else {
                return "<unsupported>";
            }
            return "<expression>";
        }

    private:
        bool _infix;
        string _op;
        CodeNodeList _arguments;
    };
    REN_SHARED_PTR(CallCodeNode);


    class NameCodeNode : public CodeNode {
    public:
        NameCodeNode(const string& name)
        : _name(name) {
        }

        string asExpression() const {
            return _name;
        }

    private:
        string _name;
    };
    REN_SHARED_PTR(NameCodeNode);

}


#endif
