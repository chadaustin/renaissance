#pragma once

#include <ren/Type.h>

namespace ren {

    struct Signature {
        Type result;
        std::vector<Type> arguments;
    };

    template<typename T> struct TypeMap;
#define REN_TYPEMAP(c, g) template<> struct TypeMap<c> { static Type get() { return g; } };
    REN_TYPEMAP(bool, BOOL);
    REN_TYPEMAP(class bvec2, BVEC2);
    REN_TYPEMAP(class bvec3, BVEC3);
    REN_TYPEMAP(class bvec4, BVEC4);
    REN_TYPEMAP(int, INT);
    REN_TYPEMAP(class ivec2, IVEC2);
    REN_TYPEMAP(class ivec3, IVEC3);
    REN_TYPEMAP(class ivec4, IVEC4);
    REN_TYPEMAP(float, FLOAT);
    REN_TYPEMAP(class vec2, VEC2);
    REN_TYPEMAP(class vec3, VEC3);
    REN_TYPEMAP(class vec4, VEC4);
    REN_TYPEMAP(class mat2, MAT2);
    REN_TYPEMAP(class mat3, MAT3);
    REN_TYPEMAP(class mat4, MAT4);
#undef REN_TYPEMAP

    template<typename T>
    Type getType() {
        return TypeMap<T>::get();
    }

    template<typename T>
    struct SignatureType;

    template<typename T>
    struct SignatureType<T()> {
        static Signature build() {
            Signature rv;
            rv.result = getType<T>();
            return rv;
        }
    };

    template<typename T, typename A>
    struct SignatureType<T(A)> {
        static Signature build() {
            Signature rv;
            rv.result = getType<T>();
            rv.arguments.push_back(getType<A>());
            return rv;
        }
    };

    template<typename T, typename A, typename B>
    struct SignatureType<T(A,B)> {
        static Signature build() {
            Signature rv;
            rv.result = getType<T>();
            rv.arguments.push_back(getType<A>());
            rv.arguments.push_back(getType<B>());
            return rv;
        }
    };

    template<typename T, typename A, typename B, typename C>
    struct SignatureType<T(A,B,C)> {
        static Signature build() {
            Signature rv;
            rv.result = getType<T>();
            rv.arguments.push_back(getType<A>());
            rv.arguments.push_back(getType<B>());
            rv.arguments.push_back(getType<C>());
            return rv;
        }
    };

    struct FunctionBase {
        FunctionBase(const char* name, const Signature& signature)
            : name(name)
            , signature(signature)
        {}

        const char* name;
        Signature signature;
    };

    template<typename SignatureT>
    struct Def : FunctionBase {
        Def(const char* name)
            : FunctionBase(name, SignatureType<SignatureT>::build())
        {}
    };

    namespace functions {
        Def<float(vec2,vec2)> dot2("dot");
        Def<float(vec3,vec3)> dot3("dot");
        Def<float(vec4,vec4)> dot4("dot");
        Def<vec4(bool,vec4,vec4)> if_("if");
    }
}
