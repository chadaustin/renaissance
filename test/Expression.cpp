#include <ren/Compiler.h>
#include "TestFramework.h"
using namespace ren;


static const string source1 =
    "gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex";

static const string source2 = "foo = (2 + 2)";


TEST(Expression) {
    ProgramPtr p1 = analyze(source1);
    CHECK(p1);
    DefinitionPtr gl_Position = p1->getDefinition("gl_Position");
    CHECK(gl_Position);

    CHECK_EQUAL(gl_Position->type, "vec4");

    ProgramPtr p2 = analyze(source2);
    CHECK(p2);
    DefinitionPtr foo = p2->getDefinition("foo");
    CHECK(foo);
    CHECK_EQUAL(foo->type, "int");
}


static const string VS =
    "void main()\n"
    "{\n"
    "  gl_Position = (gl_ModelViewProjectionMatrix) * (gl_Vertex);\n"
    "}\n";

static const string FS = "";


TEST(ExpressionCompile) {
    CompilerResult cr = compile(source1);
    CHECK_EQUAL(cr.vertexShader,   VS);
    CHECK_EQUAL(cr.fragmentShader, FS);
}
