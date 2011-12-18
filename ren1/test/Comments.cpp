#include "TestPrologue.h"


static const string source =
    "# Header comment.\n"
    " #######\n"
    "gl_Position = ftransform  # Another comment.\n"
    "foo = 1\n";


TEST(Comments) {
    ProgramPtr p = parse(source);
    CHECK(p);

    DefinitionPtr pos = p->getDefinition("gl_Position");
    CHECK(pos);

    CompilationContext cc(p);
    CHECK_EQUAL(cc.instantiate("gl_Position")->getType(), VEC4);
}


TEST(CommentsCompile) {
    static string VS =
        "void main()\n"
        "{\n"
        "  gl_Position = ftransform();\n"
        "}\n";
    static string FS = "";

    CompileResult cr = compile(source);
    CHECK_EQUAL(cr.vertexShader,   VS);
    CHECK_EQUAL(cr.fragmentShader, FS);
}
