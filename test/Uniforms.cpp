#include <ren/Compiler.h>
#include "TestFramework.h"
using namespace ren;


static const string source =
    "uniform float FloatUniform\n"
    "uniform vec3  Vec3Uniform\n"
    "uniform mat4  Mat4Uniform\n";

TEST(Uniforms) {
    ProgramPtr p = analyze(source);
    CHECK(p);
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
