#include "TestPrologue.h"


static const string source =
    "f = gl_Vertex.x\n"
    "gl_Position = vec4 f f f f\n"
    ;


TEST(Swizzle) {
    ProgramPtr p = parse(source);
    CHECK(p);

    CompilationContext cc(p);
    ConcreteNodePtr f = cc.instantiate("f");
    CHECK_EQUAL(f->getType(), FLOAT);

    ConcreteNodePtr gl_Position = cc.instantiate("gl_Position");
    CHECK_EQUAL(gl_Position->getType(), VEC4);
}


TEST(SwizzleCompile) {
    static string VS =
        "void main()\n"
        "{\n"
        "  gl_Position = vec4(gl_Vertex.x, gl_Vertex.x, gl_Vertex.x, gl_Vertex.x);\n"
        "}\n";
    static string FS = "";

    CompileResult cr = compile(source);
    CHECK_EQUAL(cr.vertexShader,   VS);
    CHECK_EQUAL(cr.fragmentShader, FS);
}
