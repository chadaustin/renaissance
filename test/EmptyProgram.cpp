#include <ren/Compiler.h>
#include "TestFramework.h"
using namespace ren;


static const string emptyProgram1 = "";
static const string emptyProgram2 = "\n  \n  \n     \n";


TEST(EmptyProgram) {
    CHECK(analyze(emptyProgram1));
    CHECK(analyze(emptyProgram2));
}
