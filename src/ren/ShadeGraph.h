#ifndef REN_SHADE_GRAPH_H
#define REN_SHADE_GRAPH_H


#include <iostream>
#include <set>
#include <map>
#include "CodeNode.h"
#include "GLSLShader.h"


namespace ren {

    class ShadeGraph {
    public:
        std::map<string, CodeNodePtr> outputs;

        void generate(GLSLShader& vs, GLSLShader& fs) {
            if (outputs.count("gl_Position")) {
                typedef std::set<ValueNodePtr> ValueNodeSet;

                // Declare referenced uniforms.
                NameCodeNodeSet uniforms;
                getUniforms(uniforms, outputs["gl_Position"]);
                for (NameCodeNodeSet::iterator i = uniforms.begin();
                     i != uniforms.end();
                     ++i
                ) {
                    GLSLShader::Uniform u;
                    u.name = (*i)->getName();
                    u.type = (*i)->getType().getName();
                    vs.uniforms.push_back(u);
                }

                // Declare referenced attributes.
                NameCodeNodeSet attributes;
                getAttributes(attributes, outputs["gl_Position"]);
                for (NameCodeNodeSet::iterator i = attributes.begin();
                     i != attributes.end();
                     ++i
                ) {
                    GLSLShader::Attribute a;
                    a.name = (*i)->getName();
                    a.type = (*i)->getType().getName();
                    vs.attributes.push_back(a);
                }

                // Add statements.
                GLSLShader::Statement s;
                s.define = false;
                s.lhs = "gl_Position";
                s.rhs = outputs["gl_Position"];
                vs.statements.push_back(s);
            }

            if (outputs.count("gl_FragColor")) {
                GLSLShader::Statement s;
                s.define = false;
                s.lhs = "gl_FragColor";
                s.rhs = outputs["gl_FragColor"];
                fs.statements.push_back(s);
            }
        }


    private:
        typedef std::set<NameCodeNodePtr> NameCodeNodeSet;

        static void getUniforms(
            NameCodeNodeSet& uniforms,
            CodeNodePtr node
        ) {
            getReferencesOfType(uniforms, node, ValueNode::UNIFORM);
        }

        static void getAttributes(
            NameCodeNodeSet& attributes,
            CodeNodePtr node
        ) {
            getReferencesOfType(attributes, node, ValueNode::ATTRIBUTE);
        }

        static void getReferencesOfType(
            NameCodeNodeSet& result,
            CodeNodePtr codeNode,
            ValueNode::InputType type
        ) {
            assert(codeNode);
            if (REN_DYNAMIC_CAST_PTR(p, CallCodeNode, codeNode)) {
                const CodeNodeList& args = p->getArguments();
                for (size_t i = 0; i < args.size(); ++i) {
                    getReferencesOfType(result, args[i], type);
                }
            } else if (REN_DYNAMIC_CAST_PTR(p, NameCodeNode, codeNode)) {
                if (p->getInputType() == type) {
                    result.insert(p);
                }
            } else {
                assert(!"Unknown Code Node");
            }
        }
    };

}


#endif
