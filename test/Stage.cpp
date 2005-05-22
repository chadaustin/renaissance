#include "TestPrologue.h"


TEST(CanInterpolate) {
    string source =
        "yes1 = 10\n"
        "yes2 = 10 + 10\n"
        "yes3 = gl_NormalMatrix * gl_Normal\n"
        "yes4 = gl_Color + gl_Vertex\n"
        "yes5 = gl_Color\n"
        "yes6 = gl_Vertex\n"

        "no1 = gl_FragCoord\n"
        "no2 = gl_Normal.normalize\n"
        "no3 = gl_Vertex * gl_Vertex\n"
        ;

    ProgramPtr p = parse(source);
    CHECK(p);
    CompilationContext cc(p);

#define YES(yes) CHECK(cc.evaluate(yes)->canInterpolate())
    YES("yes1");
    YES("yes2");
    YES("yes3");
    YES("yes4");
    YES("yes5");
    YES("yes6");

#define NO(no)   CHECK(!cc.evaluate(no)->canInterpolate())
    NO("no1");
    NO("no2");
    NO("no3");
}


TEST(Stage) {
    string source =
        "gl_Position = ftransform\n"
        "gl_FragColor = gl_ModelViewProjectionMatrix * gl_Vertex\n"
        ;

    string VS =
        "varying vec4 _ren_v0;\n"
        "void main()\n"
        "{\n"
        "  gl_Position = ftransform();\n"
        "  _ren_v0 = (gl_ModelViewProjectionMatrix * gl_Vertex);\n"
        "}\n"
        ;

    string FS =
        "varying vec4 _ren_v0;\n"
        "void main()\n"
        "{\n"
        "  gl_FragColor = _ren_v0;\n"
        "}\n"
        ;

    CompileResult cr(compile(source));
    CHECK(cr.success);
    CHECK_EQUAL(cr.vertexShader, VS);
    CHECK_EQUAL(cr.fragmentShader, FS);
}
