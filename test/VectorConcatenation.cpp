#include "TestPrologue.h"


static const string source =
    "gl_Position = 0.0 ++ 0.0 ++ 0.0 ++ 0.0"
    ;


TEST(VectorConcatenation) {
    ProgramPtr p = parse(source);
    CHECK(p);

    const string VS =
        "void main()\n"
        "{\n"
        "  gl_Position = vec4(vec3(vec2(0.0, 0.0), 0.0), 0.0);\n"
        "}\n"
        ;
    const string FS = "";

    CompileResult cr = compile(source);
    CHECK(cr.success);
    CHECK_EQUAL(cr.vertexShader, VS);
    CHECK_EQUAL(cr.fragmentShader, FS);
}
