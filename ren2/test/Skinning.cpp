#include <ren/ren.h>
using namespace ren;

int main() {
    uniform<mat4> modelMatrix;
    uniform<mat4> viewMatrix;
    uniform<mat4> projectionMatrix;
    uniform<array<vec4, 180>> bones; // 60 4x3 row-major matrices

    attribute<vec4> position;
    attribute<ivec4> influenceIndices;
    attribute<vec4> influenceWeights;

    ivec4 i = 3 * influenceIndices; // map to matrices

    vec4 iw = influenceWeights;

    // influence 1
    vec4 row0 = bones[i.x()] * iw.x();
    vec4 row1 = bones[i.x() + 1] * iw.x();
    vec4 row2 = bones[i.x() + 2] * iw.x();

    // influence 2
    row0 += bones[i.y()] * iw.y();
    row1 += bones[i.y() + 1] * iw.y();
    row2 += bones[i.y() + 2] * iw.y();
    
    // influence 3
    row0 += bones[i.z()] * iw.z();
    row1 += bones[i.z() + 1] * iw.z();
    row2 += bones[i.z() + 2] * iw.z();
    
    // influence 4
    row0 += bones[i.w()] * iw.w();
    row1 += bones[i.w() + 1] * iw.w();
    row2 += bones[i.w() + 2] * iw.w();

    // matrix multiplication
    vec4 skinnedVertex = vec4(
        dot(row0, position),
        dot(row1, position),
        dot(row2, position),
        1.0f);
    
    VertexShader2 vs;
    vs.position = projectionMatrix * viewMatrix * modelMatrix * skinnedVertex;

    printf("%s\n", generateGLSL(vs).c_str());
}
