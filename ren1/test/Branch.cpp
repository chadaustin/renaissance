#include "TestPrologue.h"


static const string branch =
    "uniform bool condition\n"
    "p1 = ftransform\n"
    "p2 = gl_ModelViewProjectionMatrix * gl_Vertex\n"
    "gl_Position = if condition then p1 else p2\n"
    ;

TEST(Branch) {
    ProgramPtr p = parse(branch);
    CHECK(p);
    DefinitionPtr def_Position = p->getDefinition("gl_Position");
    CHECK(def_Position);
    CHECK_EQUAL(def_Position->expression->toString(), "(if condition p1 p2)");

    CompilationContext cc(p);

    ConcreteNodePtr p1 = cc.instantiate("p1");
    CHECK(p1);
    CHECK_EQUAL(p1->getType(), VEC4);

    ConcreteNodePtr p2 = cc.instantiate("p2");
    CHECK(p2);
    CHECK_EQUAL(p2->getType(), VEC4);

    ConcreteNodePtr gl_Position = cc.instantiate("gl_Position");
    CHECK(gl_Position);
    CHECK_EQUAL(gl_Position->getType(), VEC4);

    CodeNodePtr code_Position = cc.evaluate(gl_Position);
    CHECK(code_Position);
}

TEST(CompileBranch) {
    static string VS =
        "uniform bool condition;\n"
        "void main()\n"
        "{\n"
        "  vec4 _ren_r0;\n"
        "  if (condition)\n"
        "    _ren_r0 = ftransform();\n"
        "  else\n"
        "    _ren_r0 = (gl_ModelViewProjectionMatrix * gl_Vertex);\n"
        "  gl_Position = _ren_r0;\n"
        "}\n"
        ;
    static string FS = "";

    CompileResult cr(compile(branch));
    CHECK(cr.success);
    CHECK_EQUAL(cr.vertexShader,   VS);
    CHECK_EQUAL(cr.fragmentShader, FS);
}
