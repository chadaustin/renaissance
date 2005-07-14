#include "TestPrologue.h"


TEST(VectorConcatenation) {
    string source =
        "gl_Position = 0.0 ++ 0.0 ++ 0.0 ++ 0.0"
        ;

    ProgramPtr p = parse(source);
    CHECK(p);

    const string VS =
        "void main()\n"
        "{\n"
        "  gl_Position = vec4(vec3(vec2(0.0, 0.0), 0.0), 0.0);\n"
        "}\n"
        ;
    const string FS = "";

    CHECK_COMPILE(source, VS, FS);
}


TEST(Regression) {
    string source =
        "color = vec3 0.0 0.0 0.0\n"
        "gl_FragColor = color ++ 0.0\n"
        ;

    const string VS =
        "varying vec3 _ren_v0;\n"
        "void main()\n"
        "{\n"
        "  _ren_v0 = vec3(0.0, 0.0, 0.0);\n"
        "}\n"
        ;
    const string FS =
        "varying vec3 _ren_v0;\n"
        "void main()\n"
        "{\n"
        "  gl_FragColor = vec4(_ren_v0, 0.0);\n"
        "}\n"
        ;

    CHECK_COMPILE(source, VS, FS);
}
