#include "TestPrologue.h"


static const string source =
    "result = 2 ++ 2"
    ;


TEST(VectorConcatenation) {
    ProgramPtr p = analyze(source);
    CHECK(p);
}
