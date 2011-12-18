#ifndef TEST_PROLOGUE_H
#define TEST_PROLOGUE_H


#include <ren/GLSLShader.h>
#include <ren/Program.h>
#include <ren/Compiler.h>
#include <ren/CompilationContext.h>
#include <ren/Input.h>
#include "TestFramework.h"
using namespace ren;


#define CHECK_COMPILE(source, VS, FS)           \
    {                                           \
        CompileResult cr(compile(source));      \
        CHECK(cr.success);                      \
        CHECK_EQUAL(VS, cr.vertexShader);       \
        CHECK_EQUAL(FS, cr.fragmentShader);     \
    }


// Work around a bug in gcc?
namespace boost {
    namespace test_tools {
        namespace tt_detail {
            using ren::operator<<;
        }
    }
}


#endif
