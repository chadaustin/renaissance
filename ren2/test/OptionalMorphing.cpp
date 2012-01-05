#include <ren/ren.h>
using namespace ren;

int main() {
    uniform<mat4> modelMatrix;
    uniform<mat4> viewMatrix;
    uniform<mat4> projectionMatrix;

    attribute<vec4> position;
    attribute<vec4> morphOffset;

    constant<bool> morphingEnabled;

    VertexShader2 vs;
    vs.position = projectionMatrix * viewMatrix * modelMatrix * if_(morphingEnabled, position + morphOffset, position);
    
    printf("// Morphing enabled\n");
    morphingEnabled = true;
    printf("%s\n", generateGLSL(vs).c_str());
    morphingEnabled = false;
    printf("// Morphing disabled\n");
    printf("%s\n", generateGLSL(vs).c_str());
}
