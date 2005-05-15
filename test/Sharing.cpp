#include "TestPrologue.h"


static const string source =
    "f = gl_Vertex.x\n"
    "gl_Position = vec4 f f f f\n"
    ;


TEST(Sharing) {
    ProgramPtr p = parse(source);
    CHECK(p);

    CompilationContext cc(p);
    ConcreteNodePtr f1(cc.instantiate("f"));
    ConcreteNodePtr f2(cc.instantiate("f"));
    CHECK_EQUAL(f1, f2);

    CodeNodePtr c1(cc.evaluate(f1));
    CodeNodePtr c2(cc.evaluate(f2));
    CHECK_EQUAL(c1, c2);

    ConcreteNodePtr pos(cc.instantiate("gl_Position"));
    //CHECK_EQUAL(pos->asStringTree(), "");

    CodeNodePtr pos_code(cc.evaluate(pos));
    REN_DYNAMIC_CAST_PTR(pos_code_call, CallCodeNode, pos_code);
    CHECK(pos_code_call);
    
    CodeNodeList args = pos_code_call->getChildren();
    CHECK_EQUAL(args.size(), 4U);
    CHECK_EQUAL(args[0], args[1]);
    CHECK_EQUAL(args[0], args[2]);
    CHECK_EQUAL(args[0], args[3]);

    ReferenceMap rm;
    countReferences(pos_code, rm, ReferencePath());
    CHECK_EQUAL(rm[pos_code].size(), 1U);
    CHECK_EQUAL(rm[args[0]].size(),  4U);

    CHECK_EQUAL(args[0], findMultiplyReferenced(pos_code, rm));

    static string VS =
        "void main()\n"
        "{\n"
        "  float _ren_r0 = gl_Vertex.x;\n"
        "  gl_Position = vec4(_ren_r0, _ren_r0, _ren_r0, _ren_r0);\n"
        "}\n";
    static string FS = "";

    CompileResult cr = compile(source);
    CHECK(cr.success);
    CHECK_EQUAL(cr.vertexShader,   VS);
    CHECK_EQUAL(cr.fragmentShader, FS);
}
