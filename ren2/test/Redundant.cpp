#include <stdio.h>
#include <ren/ren.h>
using namespace ren;

int main() {
    attribute<vec4> x;

    vec4 p = x;
    p += p;
    p *= p;

    vec4 s = x;
    s *= s;
    s += s;

    ren::VertexShader2 vs;
    vs.position = p;
    vs.color = s;

    printf("%s\n", generateGLSL(vs).c_str());
}
