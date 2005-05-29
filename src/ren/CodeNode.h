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
        virtual Frequency getFrequency() const = 0;
        virtual string asExpression() const = 0;

        // Can this node be lifted across an interpolation boundary?
        // (Fragment -> Vertex pipeline)
        virtual bool canInterpolate() const = 0;

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

        Frequency getFrequency() const {
            assert(_children.size() == 3);
            // eh? what should this really be?
            return std::max(_children[1]->getFrequency(),
                            _children[2]->getFrequency());
        }

        string asExpression() const {
            //assert(!"IfCodeNode can't directly be turned into an expression.");
            return
                "(" + _children[0]->asExpression() +
                " ? " + _children[1]->asExpression() +
                " : " + _children[2]->asExpression();
        }

        bool canInterpolate() const {
            return false;
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
            const CodeNodeList& arguments,
            Linearity linearity)
        : _type(type)
        , _callType(callType)
        , _op(op)
        , _arguments(arguments)
        , _linearity(linearity) {
        }

        Type getType() const {
            return _type;
        }

        Frequency getFrequency() const {
            assert(!_arguments.empty());
            Frequency rv = _arguments[0]->getFrequency();
            for (size_t i = 1; i < _arguments.size(); ++i) {
                rv = std::max(rv, _arguments[i]->getFrequency());
            }
            return rv;            
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

                case FunctionNode::PREFIX: {
                    assert(_arguments.size() == 1);
                    string e = _arguments[0]->asExpression();
                    return "(" + _op + e + ")";
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

        bool canInterpolate() const {
            switch (_linearity) {
                case LINEAR: {
                    bool rv = true;
                    for (size_t i = 0; i < _arguments.size(); ++i) {
                        rv = rv && _arguments[i]->canInterpolate();
                    }
                    return rv;
                }
             
                case PARTIALLY_LINEAR: {
                    bool rv = true;
                    unsigned vertexFrequencyCount = 0;
                    for (size_t i = 0; i < _arguments.size(); ++i) {
                        rv = rv && _arguments[i]->canInterpolate();
                        if (_arguments[i]->getFrequency() == VERTEX) {
                            ++vertexFrequencyCount;
                        }
                    }
                    rv = rv && (vertexFrequencyCount == 1);
                    return rv;
                }

                case NONLINEAR: {
                    return false;
                }

                default: {
                    assert(!"Unknown linearity.");
                    return false;
                }
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
        Linearity _linearity;
    };
    REN_SHARED_PTR(CallCodeNode);


    /**
     * Represents a reference to a shader input, be it built-in or
     * specified in the shader.
     */
    class NameCodeNode : public CodeNode {
    public:
        typedef ValueNode::InputType InputType;

        NameCodeNode(
            const string& name,
            Type type,
            Frequency frequency,
            ValuePtr value,
            InputType inputType)
        : _name(name)
        , _type(type)
        , _frequency(frequency)
        , _value(value)
        , _inputType(inputType) {
            assert(_frequency > CONSTANT || _value);
        }

        Type getType() const {
            return _type;
        }
        ;
        Frequency getFrequency() const {
            return _frequency;
        }

        ValuePtr getValue() const {
            return _value;
        }

        string asExpression() const {
            return _name;
        }

        bool canInterpolate() const {
            return _frequency <= VERTEX;
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
        Frequency _frequency;
        ValuePtr _value;
        InputType _inputType;

        // We need to return something...  leave it empty.
        CodeNodeList _children;
    };
    REN_SHARED_PTR(NameCodeNode);

}


#endif
