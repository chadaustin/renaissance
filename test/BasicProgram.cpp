#include <ren/Compiler.h>
#include "TestFramework.h"
using namespace ren;


static const string basicProgram1 =
    "gl_Position = ftransform";

static const string basicProgram2 =
    "gl_Position = gl_Vertex";


TEST(BasicFunction1) {
    ProgramPtr p = analyze(basicProgram1);
    CHECK(p);
    DefinitionPtr gl_Position = p->getDefinition("gl_Position");
    CHECK(gl_Position);
    DefinitionPtr gl_Vertex = p->getDefinition("gl_Vertex");
    CHECK(!gl_Vertex);

    CHECK_EQUAL(gl_Position->type, "vec4");
}


TEST(BasicFunction2) {
    ProgramPtr p = analyze(basicProgram2);
    CHECK(p);
    DefinitionPtr gl_Position = p->getDefinition("gl_Position");
    CHECK(gl_Position);
    DefinitionPtr gl_Vertex = p->getDefinition("gl_Vertex");
    CHECK(!gl_Vertex);

    CHECK_EQUAL(gl_Position->type, "vec4");
}


static const string basicVS1 =
    "void main()\n"
    "{\n"
    "  gl_Position = ftransform();\n"
    "}\n";

static const string basicVS2 =
    "void main()\n"
    "{\n"
    "  gl_Position = gl_Vertex;\n"
    "}\n";

static const string basicFS = "";


TEST(BasicCompile) {
    CompilerResult cr1 = compile(basicProgram1);
    CHECK_EQUAL(cr1.vertexShader,   basicVS1);
    CHECK_EQUAL(cr1.fragmentShader, basicFS);

    CompilerResult cr2 = compile(basicProgram2);
    CHECK_EQUAL(cr2.vertexShader,   basicVS2);
    CHECK_EQUAL(cr2.fragmentShader, basicFS);
}
