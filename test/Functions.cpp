#include <ren/Compiler.h>
#include "TestFramework.h"
using namespace ren;


static const string functions =
    "pos = ftransform\n";


TEST(Functions) {
    ProgramPtr p = analyze(functions);
    CHECK(p);

    DefinitionPtr pos = p->getDefinition("pos");
    CHECK(pos);
    CHECK_EQUAL(pos->type, VEC4);
}
