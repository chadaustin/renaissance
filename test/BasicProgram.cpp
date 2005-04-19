#include <ren/Compiler.h>
#include "TestFramework.h"
using namespace ren;


static const string basicProgram1 =
    "gl_Position = ftransform";

static const string basicProgram2 =
    "gl_Position = gl_Vertex";


TEST(ParseBasicProgram1) {
    ProgramPtr p = parse(basicProgram1);
    CHECK(p);
    DefinitionPtr gl_Position = p->getDefinition("gl_Position");
    CHECK(gl_Position);
    DefinitionPtr gl_Vertex = p->getDefinition("gl_Vertex");
    CHECK(!gl_Vertex);

    CompilationContext cc(p);
    CHECK_EQUAL(p->type, VEC4);
}


TEST(ParseBasicProgram2) {
    ProgramPtr p = parse(basicProgram2);
    CHECK(p);
    DefinitionPtr gl_Position = p->getDefinition("gl_Position");
    CHECK(gl_Position);
    DefinitionPtr gl_Vertex = p->getDefinition("gl_Vertex");
    CHECK(!gl_Vertex);

    CHECK_EQUAL(gl_Position->type, VEC4);
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
    CompileResult cr1 = compile(basicProgram1);
    CHECK_EQUAL(cr1.vertexShader,   basicVS1);
    CHECK_EQUAL(cr1.fragmentShader, basicFS);

    CompileResult cr2 = compile(basicProgram2);
    CHECK_EQUAL(cr2.vertexShader,   basicVS2);
    CHECK_EQUAL(cr2.fragmentShader, basicFS);
}
