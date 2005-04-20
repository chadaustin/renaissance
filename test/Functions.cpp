#include "TestPrologue.h"


static const string functions =
    "pos = ftransform\n";


TEST(Functions) {
    ProgramPtr p = parse(functions);
    CHECK(p);
    CHECK(p->getDefinition("pos"));

    CompilationContext cc(p);
    CHECK_EQUAL(cc.instantiate("pos")->getType(), VEC4);
}
