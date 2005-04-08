#include <ren/Compiler.h>
#include "TestFramework.h"
using namespace ren;


static const string source = "foo = gl_Vertex.xyz\n";

TEST(Swizzle) {
    ProgramPtr p = analyze(source);
    CHECK(p);
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
