#include "TestPrologue.h"


static const string constantProgram =
    "foo = 1\n"
    "bar = 0.0\n"
    "baz = 0.\n";


TEST(ConstantFunction) {
    ProgramPtr p = parse(constantProgram);
    CHECK(p);
    CHECK(p->getDefinition("foo"));
    CHECK(p->getDefinition("bar"));
    CHECK(p->getDefinition("baz"));

    CompilationContext cc(p);
    CHECK_EQUAL(cc.instantiate("foo")->getType(), INT);
    CHECK_EQUAL(cc.instantiate("bar")->getType(), FLOAT);
    CHECK_EQUAL(cc.instantiate("baz")->getType(), FLOAT);
}
