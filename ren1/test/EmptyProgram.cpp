#include "TestPrologue.h"


static const string emptyProgram1 = "";
static const string emptyProgram2 = "\n  \n  \n     \n";


TEST(EmptyProgram) {
    CHECK(parse(emptyProgram1));
    CHECK(parse(emptyProgram2));
}
