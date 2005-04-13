#include "TestPrologue.h"


static const string source =
    "gl_FragColor = gl_Color\n"
    ;


static const string vs = "";
static const string fs =
    "void main()\n"
    "{\n"
    "  gl_FragColor = gl_Color;\n"
    "}\n"
    ;

TEST(CodeGeneration) {
    CompileResult cr = compile(source);
    CHECK(cr.success);
    CHECK_EQUAL(cr.vertexShader, vs);
    CHECK_EQUAL(cr.fragmentShader, fs);    
}
