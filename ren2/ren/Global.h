#pragma once

#include <memory>

#define REN_PTR(C) typedef std::shared_ptr<class C> C##Ptr

namespace ren {
    inline void verify(bool) {
    }
}
