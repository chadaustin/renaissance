#include <ren/Compiler.h>
#include "TestFramework.h"
using namespace ren;


static const string constantProgram =
    "foo = 1\n"
    "bar = 0.0\n"
    "baz = 0.\n";


TEST(ConstantFunction) {
    ProgramPtr p = analyze(constantProgram);
    CHECK(p);

    DefinitionPtr foo = p->getDefinition("foo");
    CHECK(foo);
    CHECK_EQUAL(foo->type, INT);

    DefinitionPtr bar = p->getDefinition("bar");
    CHECK(bar);
    CHECK_EQUAL(bar->type, FLOAT);

    DefinitionPtr baz = p->getDefinition("baz");
    CHECK(baz);
    CHECK_EQUAL(baz->type, FLOAT);
}
