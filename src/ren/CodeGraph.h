#ifndef REN_CODE_GRAPH_H
#define REN_CODE_GRAPH_H


#include <map>
#include "CodeNode.h"


namespace ren {

    class CodeGraph {
    public:
        std::map<string, CodeNodePtr> outputs;

        void getUniforms(std::ostream& os, CodeNodePtr node) {
            assert(node);
            if (REN_DYNAMIC_CAST_PTR(p, CallCodeNode, node)) {
                const CodeNodeList& args = p->getArguments();
                for (size_t i = 0; i < args.size(); ++i) {
                    getUniforms(os, args[i]);
                }
            } else if (REN_DYNAMIC_CAST_PTR(p, NameCodeNode, node)) {
                ValueNodePtr v = p->getValue();
                if (v->getInputType() == ValueNode::UNIFORM) {
                    os << "uniform " << v->getType()
                       << " " << v->getName() << ";\n";
                }
            } else {
                assert(!"Unknown Code Node");
            }
        }

        void getAttributes(std::ostream& os, CodeNodePtr node) {
            assert(node);
            if (REN_DYNAMIC_CAST_PTR(p, CallCodeNode, node)) {
                const CodeNodeList& args = p->getArguments();
                for (size_t i = 0; i < args.size(); ++i) {
                    getUniforms(os, args[i]);
                }
            } else if (REN_DYNAMIC_CAST_PTR(p, NameCodeNode, node)) {
                ValueNodePtr v = p->getValue();
                if (v->getInputType() == ValueNode::ATTRIBUTE) {
                    os << "attribute " << v->getType()
                       << " " << v->getName() << ";\n";
                }
            } else {
                assert(!"Unknown Code Node");
            }
        }

    private:
        
    };

}


#endif
