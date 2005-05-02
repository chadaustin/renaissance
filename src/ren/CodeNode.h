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
        typedef FunctionNode::CallType CallType;

        CallCodeNode(
            CallType callType,
            const string& op,
            const CodeNodeList& arguments)
        : _callType(callType)
        , _op(op)
        , _arguments(arguments) {
        }

        string asExpression() const {
            switch (_callType) {
                case FunctionNode::SWIZZLE: {
                    assert(_arguments.size() == 1);
                    string arg = _arguments[0]->asExpression();
                    return  arg + "." + _op;
                }

                case FunctionNode::INFIX: {
                    assert(_arguments.size() == 2);
                    string lhs = _arguments[0]->asExpression();
                    string rhs = _arguments[1]->asExpression();
                    return "(" + lhs + " " + _op + " " + rhs + ")";
                }

                case FunctionNode::FUNCTION: {
                    std::string rv = _op + "(";
                    for (size_t i = 0; i < _arguments.size(); ++i) {
                        if (i != 0) {
                            rv += ", ";
                        }
                        rv += _arguments[i]->asExpression();
                    }
                    return rv + ")";
                }
                    
                default:
                    assert(!"Unknown Call Type");
                    return "<unknown>";
            }
        }

        const CodeNodeList& getArguments() const {
            return _arguments;
        }

    private:
        CallType _callType;
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
