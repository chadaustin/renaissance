#include "TestPrologue.h"


TEST(Sampler) {
    string source =
        "uniform sampler2D texture\n"
        "gl_Position = ftransform\n"
        "gl_FragColor = texture2D texture gl_MultiTexCoord0.xy\n"
        ;
    string VS =
        "varying vec2 _ren_v0;\n"
        "void main()\n"
        "{\n"
        "  gl_Position = ftransform();\n"
        "  _ren_v0 = gl_MultiTexCoord0.xy;\n"
        "}\n"
        ;
    string FS =
        "uniform sampler2D texture;\n"
        "varying vec2 _ren_v0;\n"
        "void main()\n"
        "{\n"
        "  gl_FragColor = texture2D(texture, _ren_v0);\n"
        "}\n"
        ;
    CHECK_COMPILE(source, VS, FS);
}
