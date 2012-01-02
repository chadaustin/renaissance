#pragma once

#include <memory>
#include <stdexcept>

#define REN_PTR(C) typedef std::shared_ptr<class C> C##Ptr

namespace ren {
    inline void verify(bool condition) {
        if (!condition) {
            fprintf(stderr, "verify failed\n");
            throw std::runtime_error("verify failed");
        }
    }

    template<typename T, typename U>
    bool check_type(U* u) {
        return (!u || dynamic_cast<T>(u) ? true : false);
    }

    template<typename T, typename U>
    T checked_cast(U* u) {
        verify(check_type<T, U>(u));
        return static_cast<T>(u);
    }
}
