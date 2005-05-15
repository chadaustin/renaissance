#include "TestPrologue.h"


const string source =
    "a = +gl_Vertex.x\n"
    "b = -gl_Vertex.y\n"
    "f = a + b\n"
    "gl_Position = vec4 f f f f\n"
    ;


TEST(Prefix) {
    ProgramPtr p = parse(source);
    CHECK(p);
}


TEST(CompilePrefix) {
    const string VS =
        "void main()\n"
        "{\n"
        "  float _ren_r0 = ((+gl_Vertex.x) + (-gl_Vertex.y));\n"
        "  gl_Position = vec4(_ren_r0, _ren_r0, _ren_r0, _ren_r0);\n"
        "}\n"
        ;
    const string FS = "";

    CompileResult cr(compile(source));
    CHECK(cr.success);
    CHECK_EQUAL(cr.vertexShader, VS);
    CHECK_EQUAL(cr.fragmentShader, FS);
}
