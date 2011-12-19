#pragma once

namespace ren {

    class mat4 {
    };

    mat4 operator*(const mat4& left, const mat4& right) {
        return mat4();
    }

    class vec4 {
    };

    vec4 operator*(const mat4& m, const vec4& v) {
        return vec4();
    }

    template<typename T>
    class attribute {
    };

    template<typename T, typename U>
    auto operator*(const attribute<T>& left, const U& right) -> decltype(T() * U()) {
        return T() * U();
    }

    template<typename T, typename U>
    auto operator*(const T& left, const attribute<U>& right) -> decltype(T() * U()) {
        return T() * U();
    }

};
