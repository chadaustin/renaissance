#include "TestPrologue.h"


static const string source =
    "error =\n"
    ;


TEST(Error) {
    // Eat the errors so they don't show up in cerr.
    std::ostringstream log;
    CompileResult cr = compile(source, log);
    CHECK(!cr.success);
}
