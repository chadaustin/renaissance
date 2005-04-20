#include "TestPrologue.h"


static const string source =
    "uniform float FloatUniform\n"
    "uniform vec4  Vec3Uniform\n"
    "uniform mat3  Mat4Uniform\n"
    "gl_Position = Vec3Uniform\n"
    ;

TEST(Uniforms) {
    ProgramPtr p = parse(source);
    CHECK(p);

    DefinitionPtr gl_Position = p->getDefinition("gl_Position");
    CHECK(gl_Position);
}


TEST(UniformsCompile) {
/*
    static string VS =
        "void main()\n"
        "{\n"
        "}\n";
    static string FS = "";

    CompilerResult cr = compile(source);
    CHECK_EQUAL(cr.vertexShader,   VS);
    CHECK_EQUAL(cr.fragmentShader, FS);
*/
}
