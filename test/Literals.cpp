#include "TestPrologue.h"


static const string source =
    "int1 = 1\n"
    "int2 = 8784768\n"
    "float1 = 21.21\n"
    "float2 = 89.888\n"
    "bool1 = true\n"
    "bool2 = false\n"
    ;


#define CHECK_TYPE(name, typen) {                               \
        CHECK(p->getDefinition(name));                          \
        CHECK_EQUAL(cc.instantiate(name)->getType(), (typen));  \
    }


TEST(BasicFunction1) {
    ProgramPtr p = parse(source);
    CHECK(p);

    CompilationContext cc(p);
    
    CHECK_TYPE("int1",   INT);
    CHECK_TYPE("int2",   INT);
    CHECK_TYPE("float1", FLOAT);
    CHECK_TYPE("float2", FLOAT);
    CHECK_TYPE("bool1",  BOOL);
    CHECK_TYPE("bool2",  BOOL);
}
