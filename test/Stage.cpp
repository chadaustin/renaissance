#include "TestPrologue.h"


TEST(Stage) {
    string source =
        "gl_Position = ftransform\n"
        "gl_FragColor = gl_ModelViewProjectionMatrix * gl_Vertex\n"
        ;

    string VS =
        "varying vec3 _ren_v0;\n"
        "void main()\n"
        "{\n"
        "  gl_Position = ftransform();\n"
        "  _ren_v0 = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
        "}\n"
        ;

    string FS =
        "varying vec3 _ren_v0;\n"
        "void main()\n"
        "{\n"
        "  gl_FragColor = _ren_v0;\n"
        "}\n"
        ;

    CompileResult cr(compile(source));
    CHECK(cr.success);
    //CHECK_EQUAL(cr.vertexShader, VS);
    //CHECK_EQUAL(cr.fragmentShader, FS);
}
