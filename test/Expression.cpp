#include <ren/Compiler.h>
#include "TestFramework.h"
using namespace ren;


static const string expressionProgram =
    "gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex";


TEST(Expression) {
    ProgramPtr p = analyze(expressionProgram);
    CHECK(p);
    DefinitionPtr gl_Position = p->getDefinition("gl_Position");
    CHECK(gl_Position);

    CHECK_EQUAL(gl_Position->type, "vec4");
}


static const string VS =
    "void main()\n"
    "{\n"
    "  gl_Position = (gl_ModelViewProjectionMatrix) * (gl_Vertex);\n"
    "}\n";

static const string FS = "";


TEST(ExpressionCompile) {
    CompilerResult cr = compile(expressionProgram);
    CHECK_EQUAL(cr.vertexShader,   VS);
    CHECK_EQUAL(cr.fragmentShader, FS);
}
