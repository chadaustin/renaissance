#include "TestPrologue.h"


TEST(Specialize) {
    string source =
        "foo = ftransform\n"
        "bar = gl_ModelViewProjectionMatrix * gl_Vertex\n"
        "gl_Position = if true then foo else bar\n"
        ;

    string VS =
        "void main()\n"
        "{\n"
        "  gl_Position = ftransform();\n"
        "}\n"
        ;

    string FS = "";

    CHECK_COMPILE(source, VS, FS);
}
