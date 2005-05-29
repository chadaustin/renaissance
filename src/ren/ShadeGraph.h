#ifndef REN_SHADE_GRAPH_H
#define REN_SHADE_GRAPH_H


#include <map>
#include "CodeNode.h"
#include "GLSLShader.h"


namespace ren {

    class ShadeGraph {
    public:
        typedef std::map<string, CodeNodePtr> OutputMap;
        OutputMap outputs;

        /// Evaluate constant-frequency computations.
        void specialize();

        void generate(GLSLShader& vs, GLSLShader& fs);

    private:
        CodeNodePtr findEvaluatable();
        void replace(CodeNodePtr node, CodeNodePtr with);

        void lift(GLSLShader& vs, GLSLShader& fs);
    };

}


#endif
