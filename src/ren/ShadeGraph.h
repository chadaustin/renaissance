#ifndef REN_SHADE_GRAPH_H
#define REN_SHADE_GRAPH_H


#include <map>
#include "CodeNode.h"
#include "GLSLShader.h"


namespace ren {

    class ShadeGraph {
    public:
        std::map<string, CodeNodePtr> outputs;

        void generate(GLSLShader& vs, GLSLShader& fs);
    };

}


#endif
