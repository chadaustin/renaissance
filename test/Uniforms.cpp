#include "TestPrologue.h"


static const string source =
    "uniform float FloatUniform\n"
    "uniform vec4  Vec4Uniform\n"
    "uniform mat3  Mat3Uniform\n"
    "gl_Position = Vec4Uniform\n"
    ;

TEST(Uniforms) {
    ProgramPtr p = parse(source);
    CHECK(p);

    DefinitionPtr gl_Position = p->getDefinition("gl_Position");
    CHECK(gl_Position);

    CompilationContext cc(p);
    ConcreteNodePtr gl_PositionNode = cc.instantiate("gl_Position");
    CHECK(gl_PositionNode);
    CHECK(gl_PositionNode->getType() == VEC4);
}


TEST(UniformsCompile) {
    static string VS =
        "uniform vec4 Vec4Uniform;\n"
        "void main()\n"
        "{\n"
        "  gl_Position = Vec4Uniform;\n"
        "}\n";
    static string FS = "";

    CompileResult cr = compile(source);
    CHECK_EQUAL(cr.vertexShader,   VS);
    CHECK_EQUAL(cr.fragmentShader, FS);
}
