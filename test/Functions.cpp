#include "TestPrologue.h"


static const string value =
    "pos = ftransform\n";


TEST(Value) {
    ProgramPtr p = parse(value);
    CHECK(p);
    CHECK(p->getDefinition("pos"));

    CompilationContext cc(p);
    CHECK_EQUAL(cc.instantiate("pos")->getType(), VEC4);
}


static const string function =
    "I x = x\n"
    "gl_Position = I ftransform\n"
    ;

TEST(Function) {
    ProgramPtr p = parse(function);
    CHECK(p);
    CHECK(!p->getDefinition("I" /* no arguments */));

    DefinitionPtr gl_Position = p->getDefinition("gl_Position");
    CHECK(gl_Position);
    CHECK_EQUAL(gl_Position->name, "gl_Position");

    ArgumentList args;
    CHECK_EQUAL(gl_Position->arguments, args);
    CHECK(gl_Position->expression);
    CHECK_EQUAL(gl_Position->expression->toString(), "(I ftransform)");

    CompilationContext cc(p);
    CHECK_EQUAL(cc.instantiate("gl_Position")->getType(), VEC4);
}

static const string functionVS =
    "void main()\n"
    "{\n"
    "  gl_Position = ftransform();\n"
    "}\n"
    ;
static const string functionFS = "";


TEST(CompileFunction) {
//    CompileResult cr = compile(function);
//    CHECK(cr.success);
//    CHECK_EQUAL(cr.vertexShader,   functionVS);
//    CHECK_EQUAL(cr.fragmentShader, functionFS);
}
