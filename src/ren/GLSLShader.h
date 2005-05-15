#ifndef GLSL_SHADER_H
#define GLSL_SHADER_H


#include <iostream>
#include <map>
#include <vector>
#include "Base.h"
#include "CodeNode.h"
#include "GLSLStatement.h"


namespace ren {

    typedef std::vector<StatementPtr> ReferencePath;
    typedef std::vector<ReferencePath> ReferenceList;
    typedef std::map<CodeNodePtr, ReferenceList> ReferenceMap;

    extern void countReferences(
        CodeNodePtr node,
        ReferenceMap& refs,
        const ReferencePath& path);

    extern void countReferences(
        StatementPtr statement,
        ReferenceMap& refs);

    extern CodeNodePtr findMultiplyReferenced(
        CodeNodePtr node,
        const ReferenceMap& refs);


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

        std::vector<Uniform>   uniforms;
        std::vector<Attribute> attributes;
        BlockPtr main;

        GLSLShader();

        void generate(std::ostream& os);

    private:
        string newRegisterName();
        void splitBranches();
        void share();

        int _register;
    };

}


#endif
