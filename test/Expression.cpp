#include "TestPrologue.h"


static const string source1 =
    "gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex";

static const string source2 = "foo = (2 + 2)";


TEST(Expression) {
    ProgramPtr p1 = parse(source1);
    CHECK(p1);
    DefinitionPtr gl_Position = p1->getDefinition("gl_Position");
    CHECK(gl_Position);

    CompilationContext cc1(p1);
    CHECK_EQUAL(cc1.instantiate("gl_Position")->getType(), VEC4);

    ProgramPtr p2 = parse(source2);
    CHECK(p2);
    DefinitionPtr foo = p2->getDefinition("foo");
    CHECK(foo);

    CompilationContext cc2(p2);
    CHECK_EQUAL(cc2.instantiate("foo")->getType(), INT);
}


static const string VS =
    "void main()\n"
    "{\n"
    "  gl_Position = (gl_ModelViewProjectionMatrix * gl_Vertex);\n"
    "}\n";

static const string FS = "";


TEST(ExpressionCompile) {
    CompileResult cr = compile(source1);
    CHECK_EQUAL(cr.vertexShader,   VS);
    CHECK_EQUAL(cr.fragmentShader, FS);
}
