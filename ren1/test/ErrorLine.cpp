#include <sstream>
#include "TestPrologue.h"


static const string error_line1 = "error";
static const string error_line2 = "\nerror";
static const string error_line3 = "\n#comment\nerror # foo";


TEST(ErrorLine) {
    std::ostringstream log1;
    CompileResult cr = compile(error_line1, log1);
    CHECK(!cr.success);

    CHECK_EQUAL(log1.str(),
                "ANTLR Exception: line 1:6: expecting IS, found ''\n");


    std::ostringstream log2;
    cr = compile(error_line2, log2);
    CHECK(!cr.success);

    CHECK_EQUAL(log2.str(),
                "ANTLR Exception: line 2:6: expecting IS, found ''\n");

    std::ostringstream log3;
    cr = compile(error_line3, log3);
    CHECK(!cr.success);

    CHECK_EQUAL(log3.str(),
                "ANTLR Exception: line 3:12: expecting IS, found ''\n");
}
