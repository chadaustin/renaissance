#include "TestPrologue.h"


static const string error =
    "error =\n"
    ;


TEST(Error) {
    // Eat the errors so they don't show up in cerr.
    std::ostringstream log;
    CompileResult cr = compile(error, log);
    CHECK(!cr.success);
}


static const string recursive =
    "pos = pos\n"
    "gl_Position = pos\n"
    ;


TEST(Recursive) {
    ProgramPtr p = parse(recursive);
    CHECK(p);

    CompilationContext cc(p);
    try {
        ConcreteNodePtr gl_Vertex = cc.instantiate("pos");
        CHECK(!"pos is recursive definition");
    }
    catch (const CompileError& e) {
        CHECK("Caught recursive definition error");
    }

    std::ostringstream log;
    CompileResult cr(compile(recursive, log));
    CHECK(!cr.success);
    CHECK_EQUAL(log.str(),
                "Exception: Recursion not allowed in definition of pos\n");
}
