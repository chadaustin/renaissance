#include <sstream>
#include <ren/Compiler.h>
#include "TestFramework.h"
using namespace ren;


static const string error_line1 = "error";
static const string error_line2 = "\nerror";
static const string error_line3 = "\n#comment\nerror # foo";


TEST(ErrorLine) {
    std::ostringstream log1;
    ProgramPtr p1 = analyze(error_line1, log1);
    CHECK(!p1);

    CHECK_EQUAL(log1.str(),
                "ANTLR Exception: line 1:6: expecting IS, found ''\n");


    std::ostringstream log2;
    ProgramPtr p2 = analyze(error_line2, log2);
    CHECK(!p2);

    CHECK_EQUAL(log2.str(),
                "ANTLR Exception: line 2:6: expecting IS, found ''\n");

    std::ostringstream log3;
    ProgramPtr p3 = analyze(error_line3, log3);
    CHECK(!p3);

    CHECK_EQUAL(log3.str(),
                "ANTLR Exception: line 3:12: expecting IS, found ''\n");
}
