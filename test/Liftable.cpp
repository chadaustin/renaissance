#include "TestPrologue.h"


TEST(Liftable) {
    string source =
        "gl_Position = ftransform\n"
        "gl_FragColor = gl_Color\n"
        ;
    string VS =
        "varying vec4 _ren_v0;\n"
        "void main()\n"
        "{\n"
        "  gl_Position = ftransform();\n"
        "  _ren_v0 = gl_Color;\n"
        "}\n"
        ;
    string FS =
        "varying vec4 _ren_v0;\n"
        "void main()\n"
        "{\n"
        "  gl_FragColor = _ren_v0;\n"
        "}\n"
        ;
    CHECK_COMPILE(source, VS, FS);
}
