#include "TestPrologue.h"


static const string source =
    "int1 = 1\n"
    "int2 = 8784768\n"
    "float1 = 21.21\n"
    "float2 = 89.888\n"
    "bool1 = true\n"
    "bool2 = false\n"
    ;


#define CHECK_TYPE(name, typen) {                       \
        DefinitionPtr d = p->getDefinition(name);       \
        CHECK(d);                                       \
        CHECK_EQUAL(d->type, (typen));                  \
    }


TEST(BasicFunction1) {
    ProgramPtr p = analyze(source);
    CHECK(p);
    CHECK_TYPE("int1",   "int");
    CHECK_TYPE("int2",   "int");
    CHECK_TYPE("float1", "float");
    CHECK_TYPE("float2", "float");
    CHECK_TYPE("bool1",  "bool");
    CHECK_TYPE("bool2",  "bool");
}
