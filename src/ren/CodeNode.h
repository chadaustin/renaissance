#ifndef REN_CODE_NODE_H
#define REN_CODE_NODE_H


#include "ConcreteNode.h"
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

        const CodeNodeList& getArguments() const {
            return _arguments;
        }

    private:
        bool _infix;
        string _op;
        CodeNodeList _arguments;
    };
    REN_SHARED_PTR(CallCodeNode);


    class NameCodeNode : public CodeNode {
    public:
        NameCodeNode(const string& name, ValueNodePtr value)
        : _name(name)
        , _value(value) {
        }

        string asExpression() const {
            return _name;
        }

        ValueNodePtr getValue() const {
            return _value;
        }

    private:
        string _name;
        ValueNodePtr _value;
    };
    REN_SHARED_PTR(NameCodeNode);

}


#endif
