#include "TestPrologue.h"


static const string source =
    "gl_Position = gl_Vertex.wxyz\n"
    ;


TEST(Swizzle) {
    ProgramPtr p = parse(source);
    CHECK(p);

    CompilationContext cc(p);

    ConcreteNodePtr gl_Position = cc.instantiate("gl_Position");
    CHECK_EQUAL(gl_Position->getType(), VEC4);
}


TEST(SwizzleCompile) {
    static string VS =
        "void main()\n"
        "{\n"
        "  gl_Position = gl_Vertex.wxyz;\n"
        "}\n";
    static string FS = "";

    CompileResult cr = compile(source);
    CHECK_EQUAL(cr.vertexShader,   VS);
    CHECK_EQUAL(cr.fragmentShader, FS);
}
