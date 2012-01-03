#include <stdio.h>
#include <ren/ren.h>
using namespace ren;

int main() {
    constant<float> c;

    ren::VertexShader2 vs;
    vs.position = vec4(c, c, c, c);
    vs.color = vec4(1.0f, 2.0f, 3.0f, 4.0f);

    printf("%s\n", generateGLSL(vs).c_str());
}
