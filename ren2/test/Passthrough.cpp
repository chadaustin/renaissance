#include <stdio.h>
#include <ren/ren.h>
using namespace ren;

int main() {
    attribute<vec4> position;

    ren::VertexShader2 vs;
    vs.output.position = position;

    printf("%s\n", vs.generateGLSL().c_str());
}
