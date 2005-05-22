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

    extern CodeNodePtr findInterpolatable(CodeNodePtr node);
    extern CodeNodePtr findInterpolatable(StatementPtr statement);

    extern void replace(StatementPtr st, CodeNodePtr node, CodeNodePtr with);


    class GLSLShader {
    public:
        struct Global {
            string name;
            string type;
        };

        struct Uniform   : Global { };
        struct Attribute : Global { };
        struct Varying   : Global { };

        std::vector<Uniform>   uniforms;
        std::vector<Attribute> attributes;
        std::vector<Varying>   varyings;
        BlockPtr main;

        GLSLShader();

        void generate(std::ostream& os);
        void output(std::ostream& os);

        string newVaryingName();

    private:
        string newRegisterName();
        void splitBranches();
        void share();

        unsigned _varying;
        unsigned _register;
    };

}


#endif
