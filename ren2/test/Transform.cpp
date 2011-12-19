#include <stdio.h>
#include <ren/ren.h>
using namespace ren;

int main() {
    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 projectionMatrix;

    attribute<vec4> position;

    ren::VertexShader2 vs;
    vs.output.position = projectionMatrix * viewMatrix * modelMatrix * position;

    printf("%s\n", vs.generateGLSL().c_str());
}
