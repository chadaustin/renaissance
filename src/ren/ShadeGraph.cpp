#include <iostream>
#include <set>
#include "ShadeGraph.h"


namespace {
    using namespace ren;

    typedef std::set<NameCodeNodePtr> NameCodeNodeSet;

    void getReferencesOfType(
        NameCodeNodeSet& result,
        CodeNodePtr codeNode,
        ValueNode::InputType type
    ) {
        assert(codeNode);
        if (REN_DYNAMIC_CAST_PTR(p, CallCodeNode, codeNode)) {
            CodeNodeList args = p->getChildren();
            for (size_t i = 0; i < args.size(); ++i) {
                getReferencesOfType(result, args[i], type);
            }
        } else if (REN_DYNAMIC_CAST_PTR(p, IfCodeNode, codeNode)) {
            CodeNodeList args = p->getChildren();
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

    void getUniforms(NameCodeNodeSet& uniforms, CodeNodePtr node) {
        getReferencesOfType(uniforms, node, ValueNode::UNIFORM);
    }

    void getAttributes(NameCodeNodeSet& attributes, CodeNodePtr node) {
        getReferencesOfType(attributes, node, ValueNode::ATTRIBUTE);
    }

}


namespace ren {

    void ShadeGraph::generate(GLSLShader& vs, GLSLShader& fs) {
        if (outputs.count(Output("gl_Position"))) {
            CodeNodePtr cn = outputs[Output("gl_Position")];

            typedef std::set<ValueNodePtr> ValueNodeSet;

            // Declare referenced uniforms.
            NameCodeNodeSet uniforms;
            getUniforms(uniforms, cn);
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
            getAttributes(attributes, cn);
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
            AssignmentPtr s(new Assignment);
            s->define = false;
            s->lhs = "gl_Position";
            s->rhs = cn;
            vs.main->statements.push_back(s);
        }

        if (outputs.count(Output("gl_FragColor"))) {
            CodeNodePtr cn = outputs[Output("gl_FragColor")];

            // Declare referenced uniforms.
            NameCodeNodeSet uniforms;
            getUniforms(uniforms, cn);
            for (NameCodeNodeSet::iterator i = uniforms.begin();
                 i != uniforms.end();
                 ++i
            ) {
                GLSLShader::Uniform u;
                u.name = (*i)->getName();
                u.type = (*i)->getType().getName();
                fs.uniforms.push_back(u);
            }

            // Declare referenced attributes.
            NameCodeNodeSet attributes;
            getAttributes(attributes, cn);
            for (NameCodeNodeSet::iterator i = attributes.begin();
                 i != attributes.end();
                 ++i
            ) {
                GLSLShader::Attribute a;
                a.name = (*i)->getName();
                a.type = (*i)->getType().getName();
                fs.attributes.push_back(a);
            }

            // Add statements.
            AssignmentPtr s(new Assignment);
            s->define = false;
            s->lhs = "gl_FragColor";
            s->rhs = cn;
            fs.main->statements.push_back(s);
        }

        lift(vs, fs);
    }


    void ShadeGraph::lift(GLSLShader& vs, GLSLShader& fs) {
        StatementPtr main_stmt(fs.main);

/*
        std::cout << "########\n";
        vs.output(std::cout);
        std::cout << "--------\n";
        fs.output(std::cout);
*/

        while (CodeNodePtr varying = findInterpolatable(main_stmt)) {
            string name = fs.newVaryingName();

            GLSLShader::Varying v;
            v.name = name;
            v.type = varying->getType().getName();

            // Link shaders through a varying.
            vs.varyings.push_back(v);
            fs.varyings.push_back(v);

            // Define the varying in the vertex shader.
            AssignmentPtr assignVarying(new Assignment);
            assignVarying->define = false;
            assignVarying->lhs = name;
            assignVarying->rhs = varying;
            vs.main->statements.push_back(assignVarying);

            // Reference the varying from the fragment shader.
            CodeNodePtr varyingReference(
                    new NameCodeNode(
                        name,
                        varying->getType(),
                        varying->getFrequency(),
                        ValueNode::BUILTIN)); // suitable substitute for local
            replace(main_stmt, varying, varyingReference);

/*
            std::cout << "########\n";
            vs.output(std::cout);
            std::cout << "--------\n";
            fs.output(std::cout);
*/
        }
    }

}
