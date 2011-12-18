#include "TestPrologue.h"


TEST(Frequency) {
    string source =
        "foo = 2 + 2\n"
        "cv = vec4 0.0 0.0 0.0 0.0\n"
        "gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex\n"
        ;

    ProgramPtr p(parse(source));
    CompilationContext cc(p);

#define freqOf(name) cc.instantiate(name)->getFrequency()
    CHECK_EQUAL(freqOf("gl_Vertex"), VERTEX);
    CHECK_EQUAL(freqOf("gl_ModelViewProjectionMatrix"), UNIFORM);
    CHECK_EQUAL(freqOf("foo"), CONSTANT);
    CHECK_EQUAL(freqOf("cv"), CONSTANT);
    CHECK_EQUAL(freqOf("gl_Position"), VERTEX);

    CodeNodePtr gl_Position = cc.evaluate("gl_Position");
    CHECK_EQUAL(gl_Position->getFrequency(), VERTEX);
}
