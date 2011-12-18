#include "TestPrologue.h"


static const string ref_source =
    "position = gl_Vertex\n"
    "gl_Position = position\n"
    ;

static const string ref_vs =
    "void main()\n"
    "{\n"
    "  gl_Position = gl_Vertex;\n"
    "}\n"
    ;
static const string ref_fs = "";


TEST(ReferenceGeneration) {
    CompileResult cr = compile(ref_source);
    CHECK(cr.success);
    CHECK_EQUAL(cr.vertexShader, ref_vs);
    CHECK_EQUAL(cr.fragmentShader, ref_fs);
}
