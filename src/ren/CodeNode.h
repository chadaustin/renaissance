#ifndef REN_CODE_NODE_H
#define REN_CODE_NODE_H


#include "ConcreteNode.h"
#include "Types.h"


namespace ren {

    class CodeNode;
    REN_SHARED_PTR(CodeNode);
    typedef std::vector<CodeNodePtr> CodeNodeList;


    class CodeNode {
    public:
        CodeNode() {
        }

        virtual ~CodeNode() { }

        virtual Type getType() const = 0;
        virtual string asExpression() const = 0;

        /// Don't resize the list returned by this method.
        virtual CodeNodeList& getChildren() = 0;
    };


    /// Represents a conditional.
    class IfCodeNode : public CodeNode {
    public:
        IfCodeNode(
            Type type,
            CodeNodePtr condition,
            CodeNodePtr truePart,
            CodeNodePtr falsePart)
        : _type(type)
        , _children(3) {
            _children[0] = condition;
            _children[1] = truePart;
            _children[2] = falsePart;
        }

        Type getType() const {
            return _type;
        }

        string asExpression() const {
            //assert(!"IfCodeNode can't directly be turned into an expression.");
            return
                "(" + _children[0]->asExpression() +
                " ? " + _children[1]->asExpression() +
                " : " + _children[2]->asExpression();
        }


        CodeNodeList& getChildren() {
            return _children;
        }

    private:
        Type _type;
        CodeNodeList _children;
    };
    REN_SHARED_PTR(IfCodeNode);


    /// Represents a call to a native function.
    class CallCodeNode : public CodeNode {
    public:
        typedef FunctionNode::CallType CallType;

        CallCodeNode(
            Type type,
            CallType callType,
            const string& op,
            const CodeNodeList& arguments)
        : _type(type)
        , _callType(callType)
        , _op(op)
        , _arguments(arguments) {
        }

        Type getType() const {
            return _type;
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

        CodeNodeList& getChildren() {
            return _arguments;
        }

    private:
        Type _type;
        CallType _callType;
        string _op;
        CodeNodeList _arguments;
    };
    REN_SHARED_PTR(CallCodeNode);


    /**
     * Represents a reference to a shader input, be it built-in or
     * specified in the shader.
     */
    class NameCodeNode : public CodeNode {
    public:
        typedef ValueNode::InputType InputType;

        NameCodeNode(const string& name, Type type, InputType inputType)
        : _name(name)
        , _type(type)
        , _inputType(inputType) {
        }

        Type getType() const {
            return _type;
        }

        string asExpression() const {
            return _name;
        }

        CodeNodeList& getChildren() {
            return _children;
        }

        string getName() const {
            return _name;
        }

        InputType getInputType() const {
            return _inputType;
        }

    private:
        string _name;
        Type _type;
        InputType _inputType;

        // We need to return something...  leave it empty.
        CodeNodeList _children;
    };
    REN_SHARED_PTR(NameCodeNode);

}


#endif
