#include "TestPrologue.h"


TEST(ConstantSpecialize) {
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


TEST(Specialize) {
    string source =
        "constant bool condition\n"
        "foo = ftransform\n"
        "bar = gl_ModelViewProjectionMatrix * gl_Vertex\n"
        "gl_Position = if condition then foo else bar\n"
        ;

    string VStrue =
        "void main()\n"
        "{\n"
        "  gl_Position = ftransform();\n"
        "}\n"
        ;

    string VSfalse =
        "void main()\n"
        "{\n"
        "  gl_Position = (gl_ModelViewProjectionMatrix * gl_Vertex);\n"
        "}\n"
        ;

    string FS = "";

    ProgramPtr p = parse(source);
    CHECK(p);
    Bool condition(p, "condition");

    CHECK_COMPILE(p, VSfalse, FS);
    condition = true;
    CHECK_COMPILE(p, VStrue, FS);
    condition = false;
    CHECK_COMPILE(p, VSfalse, FS);
}
