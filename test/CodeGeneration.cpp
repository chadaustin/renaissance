#include "TestPrologue.h"


static const string frag_source =
    "gl_FragColor = gl_Color\n"
    ;


static const string frag_vs = "";
static const string frag_fs =
    "void main()\n"
    "{\n"
    "  gl_FragColor = gl_Color;\n"
    "}\n"
    ;

TEST(FragmentProgram) {
    CompileResult cr = compile(frag_source);
    CHECK(cr.success);
    CHECK_EQUAL(cr.vertexShader, frag_vs);
    CHECK_EQUAL(cr.fragmentShader, frag_fs);    
}


static const string ref_source =
    "position = gl_Vertex\n"
    "gl_Position = position\n"
    ;

static const string ref_vs =
    "void main()\n"
    "{\n"
    "  vec4 position = gl_Vertex;\n"
    "  gl_Position = position;\n"
    "}\n"
    ;
static const string ref_fs = "";


TEST(ReferenceGeneration) {
    CompileResult cr = compile(ref_source);
    CHECK(cr.success);
//    CHECK_EQUAL(cr.vertexShader, ref_vs);
//    CHECK_EQUAL(cr.fragmentShader, ref_fs);
}
