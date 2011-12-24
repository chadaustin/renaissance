#include <stdio.h>
#include <ren/ren.h>
using namespace ren;

int main() {
    uniform<mat4> modelMatrix;
    uniform<mat4> viewMatrix;
    uniform<mat4> projectionMatrix;

    attribute<vec4> position;
    attribute<vec4> color;
    attribute<vec4> secondaryColor;

    ren::VertexShader2 vs;
    vs.position = projectionMatrix * viewMatrix * modelMatrix * position;
    vs.texCoord[0] = color;
    vs.texCoord[4] = secondaryColor;

    printf("%s\n", generateGLSL(vs).c_str());
}
