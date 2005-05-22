#ifndef REN_SHADE_GRAPH_H
#define REN_SHADE_GRAPH_H


#include <map>
#include "CodeNode.h"
#include "GLSLShader.h"


namespace ren {

    class ShadeGraph {
    public:
        struct Output {
            Output(const string& n)
            : name(n) {
            }

            string name;

            bool operator<(const Output& rhs) const {
                return name < rhs.name;
            }
        };

        std::map<Output, CodeNodePtr> outputs;

        void generate(GLSLShader& vs, GLSLShader& fs);

    private:
        void lift(GLSLShader& vs, GLSLShader& fs);
    };

}


#endif
