#include <stdio.h>
#include <ren/ren.h>
using namespace ren;

int main() {
    attribute<vec4> p1;
    attribute<vec4> p2;
    constant<bool> s;

    ren::VertexShader2 vs;
    vs.position = if_(s, p1, p2);

    printf("%s\n", generateGLSL(vs).c_str());
    s = true;
    printf("%s\n", generateGLSL(vs).c_str());
}
