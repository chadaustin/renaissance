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

//    ConcreteNodePtr gl_Position = cc.instantiate("gl_Position");
//    CHECK(gl_Position);
//    CHECK_EQUAL(gl_Position->getType(), VEC4);
}
