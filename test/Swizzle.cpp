#include "TestPrologue.h"


static const string source = "foo = gl_Vertex.xyz\n";


TEST(Swizzle) {
    ProgramPtr p = parse(source);
    CHECK(p);

    CompilationContext cc(p);
    ConcreteNodePtr foo = cc.instantiate("foo");
    CHECK_EQUAL(foo->getType(), VEC3);
}


TEST(SwizzleCompile) {
/*
    static string VS =
        "void main()\n"
        "{\n"
        "}\n";
    static string FS = "";

    CompilerResult cr = compile(source);
    CHECK_EQUAL(cr.vertexShader,   VS);
    CHECK_EQUAL(cr.fragmentShader, FS);
*/
}
