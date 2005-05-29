#include "TestPrologue.h"


TEST(Constant) {
    string source =
        "constant vec4 pos\n"
        "gl_Position = pos\n"
        ;

    ProgramPtr p = parse(source);
    CHECK(p);
    CHECK(p->getDefinition("gl_Position"));

    string VS =
        "const vec4 pos = vec4(0.0, 0.0, 0.0, 0.0);\n"
        "void main()\n"
        "{\n"
        "  gl_Position = pos;\n"
        "}\n"
        ;

    string FS = "";

    CHECK_COMPILE(source, VS, FS);
}
