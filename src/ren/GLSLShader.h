#ifndef GLSL_SHADER_H
#define GLSL_SHADER_H


#include <iostream>
#include "Base.h"
#include "CodeNode.h"


namespace ren {

    extern void countReferences(CodeNodePtr node);
    extern CodeNodePtr findMultiplyReferenced(CodeNodePtr node);

    class GLSLShader {
    public:
        struct Uniform {
            string name;
            string type;
        };

        struct Attribute {
            string name;
            string type;
        };

        struct Statement {
            bool define;
            string lhs;
            CodeNodePtr rhs;
        };

        std::vector<Uniform>   uniforms;
        std::vector<Attribute> attributes;
        std::vector<Statement> statements;

        GLSLShader();

        void optimize();
        void generate(std::ostream& os);

    private:
        static void clearReferences(CodeNodePtr node) {
            assert(node);
            node->referenceCount = 0;
            if (REN_DYNAMIC_CAST_PTR(p, CallCodeNode, node)) {
                const CodeNodeList& args = p->getArguments();
                for (size_t i = 0; i < args.size(); ++i) {
                    clearReferences(args[i]);
                }
            } else if (REN_DYNAMIC_CAST_PTR(p, NameCodeNode, node)) {
                // Do nothing.
            } else {
                assert(!"Unknown code node type");
            }
        }

        int _register;
    };

}


#endif
