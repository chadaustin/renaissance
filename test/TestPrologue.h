#ifndef TEST_PROLOGUE_H
#define TEST_PROLOGUE_H


#include <ren/Program.h>
#include <ren/Compiler.h>
#include <ren/CompilationContext.h>
#include "TestFramework.h"
using namespace ren;


// Work around a bug in gcc?
namespace boost {
    namespace test_tools {
        namespace tt_detail {
            using ren::operator<<;
        }
    }
}


#endif
