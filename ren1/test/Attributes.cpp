#include "TestPrologue.h"


static const string attributes =
    "attribute vec4 pos\n"
    "gl_Position = pos\n"
    ;

TEST(Attributes) {
    ProgramPtr p = parse(attributes);
    CHECK(p);

    CompilationContext cc(p);
    ConcreteNodePtr gl_Position = cc.instantiate("gl_Position");
    CHECK(gl_Position);
    CHECK(gl_Position->getType() == VEC4);

    const string VS =
        "attribute vec4 pos;\n"
        "void main()\n"
        "{\n"
        "  gl_Position = pos;\n"
        "}\n"
        ;
    const string FS = "";

    CompileResult cr = compile(attributes);
    CHECK(cr.success);
    CHECK_EQUAL(cr.vertexShader,   VS);
    CHECK_EQUAL(cr.fragmentShader, FS);
}
