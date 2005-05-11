#include <sstream>
#include "GLSLShader.h"


namespace ren {

    void countReferences(CodeNodePtr node) {
        assert(node);
        ++node->referenceCount;
        if (REN_DYNAMIC_CAST_PTR(p, CallCodeNode, node)) {
            const CodeNodeList& args = p->getArguments();
            for (size_t i = 0; i < args.size(); ++i) {
                countReferences(args[i]);
            }
        } else if (REN_DYNAMIC_CAST_PTR(p, NameCodeNode, node)) {
            // Do nothing.
        } else {
            assert(!"Unknown code node type");
        }
    }

    CodeNodePtr findMultiplyReferenced(CodeNodePtr node) {
        assert(node);
        if (REN_DYNAMIC_CAST_PTR(p, CallCodeNode, node)) {
            if (node->referenceCount > 1) {
                return p;
            }

            const CodeNodeList& args = p->getArguments();
            for (size_t i = 0; i < args.size(); ++i) {
                CodeNodePtr c = findMultiplyReferenced(args[i]);
                if (c) {
                    return c;
                }
            }

        } else if (REN_DYNAMIC_CAST_PTR(p, NameCodeNode, node)) {
            // Do nothing.
        } else {
            assert(!"Unknown code node type");
        }
        return CodeNodePtr();
    }

    void replace(CodeNodePtr in, CodeNodePtr node, CodeNodePtr with) {
        assert(in);
        assert(node);
        assert(with);
        if (REN_DYNAMIC_CAST_PTR(in_call, CallCodeNode, in)) {
            CodeNodeList& args = in_call->getArguments();
            for (size_t i = 0; i < args.size(); ++i) {
                if (args[i] == node) {
                    args[i] = with;
                } else {
                    replace(args[i], node, with);
                }
            }
        } else if (REN_DYNAMIC_CAST_PTR(in_name, NameCodeNode, in)) {
            // Do nothing.
        } else {
            assert(!"Unknown code node type");
        }
    }

}


namespace ren {

    GLSLShader::GLSLShader()
    : _register(0) {
    }

    void GLSLShader::optimize() {
        for (size_t i = 0; i < statements.size(); ++i) {
            Statement& s = statements[i];
            clearReferences(s.rhs);
            countReferences(s.rhs);

            bool backtrack = false;
            while (CodeNodePtr p = findMultiplyReferenced(s.rhs)) {
                Statement ns;
                ns.define = true;

                std::ostringstream os;
                os << "_ren_r" << _register;
                std::string name = os.str();

                CodeNodePtr nameReference(
                    new NameCodeNode(
                        name,
                        p->getType(),
                        ValueNode::BUILTIN)); // suitable substitute for local
                replace(s.rhs, p, nameReference);

                ns.lhs = os.str();
                ns.rhs = p;
                statements.insert(statements.begin() + i, ns);
                backtrack = true;
            }
            if (backtrack) {
                --i;
            }
        }
    }

    void GLSLShader::generate(std::ostream& os) {
        optimize();

        for (size_t i = 0; i < uniforms.size(); ++i) {
            os << "uniform " << uniforms[i].type
               << " " << uniforms[i].name << ";\n";
        }

        for (size_t i = 0; i < attributes.size(); ++i) {
            os << "attribute " << attributes[i].type
               << " " << attributes[i].name << ";\n";
        }

        if (!statements.empty()) {
            os << "void main()\n"
               << "{\n";
            for (size_t i = 0; i < statements.size(); ++i) {
                os << "  ";
                Statement& s = statements[i];
                if (s.define) {
                    os << s.rhs->getType() << " ";
                }
                os << s.lhs << " = " << s.rhs->asExpression() << ";\n";
            }
            os << "}\n";
        }
    }

}
