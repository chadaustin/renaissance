#include <stdio.h>
#include <ren/ren.h>
using namespace ren;

int main() {
    uniform<mat4> modelMatrix;
    uniform<mat4> viewMatrix;
    uniform<mat4> projectionMatrix;

    attribute<vec4> position;

    ren::VertexShader2 vs;
    vs.output.position = projectionMatrix * viewMatrix * modelMatrix * position;

    printf("%s\n", vs.generateGLSL().c_str());
}