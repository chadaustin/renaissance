#define BOOST_AUTO_TEST_MAIN
#include <ren/Compiler.h>
#include "TestFramework.h"
using namespace ren;


static const string emptyProgram1 = "";
static const string emptyProgram2 = "\n\n\n\n";


TEST(EmptyProgram) {
    CHECK(analyze(emptyProgram1));
    CHECK(analyze(emptyProgram2));
}


static const string constantProgram = "foo = 1";


TEST(ConstantFunction) {
    ProgramPtr p = analyze(constantProgram);
    CHECK(p);
    DefinitionPtr foo = p->getDefinition("foo");
    CHECK(foo);
    CHECK_EQUAL(foo->type, "int");
}


static const string basicProgram = "gl_Position = gl_Vertex";


TEST(BasicFunction) {
    ProgramPtr p = analyze(basicProgram);
    CHECK(p);
    DefinitionPtr gl_Position = p->getDefinition("gl_Position");
    CHECK(gl_Position);
    DefinitionPtr gl_Vertex = p->getDefinition("gl_Vertex");
    CHECK(!gl_Vertex);

    CHECK_EQUAL(gl_Position->type, "vec4");
}


static const string basicVS =
    "void main()\n"
    "{\n"
    "  gl_Position = gl_Vertex;\n"
    "}\n";

static const string basicFS = "";


TEST(BasicCompile) {
    CompilerResult result = compile(basicProgram);
    CHECK_EQUAL(result.vertexShader,   basicVS);
    CHECK_EQUAL(result.fragmentShader, basicFS);
}
