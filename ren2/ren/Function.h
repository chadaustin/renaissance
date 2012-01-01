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

    template<typename T, typename A, typename B, typename C, typename D>
    struct SignatureType<T(A,B,C,D)> {
        static Signature build() {
            Signature rv;
            rv.result = getType<T>();
            rv.arguments.push_back(getType<A>());
            rv.arguments.push_back(getType<B>());
            rv.arguments.push_back(getType<C>());
            rv.arguments.push_back(getType<D>());
            return rv;
        }
    };

    struct FunctionBase {
        FunctionBase(const char* glsl_name, const Signature& signature)
            : glsl_name(glsl_name)
            , signature(signature)
        {}

        const char* glsl_name;
        Signature signature;
    };

    template<typename SignatureT>
    struct Function : FunctionBase {
        Function(const char* name)
            : FunctionBase(name, SignatureType<SignatureT>::build())
        {}
    };

    Function<float(vec3,vec3)> dot3("dot");
    Function<float(vec4,vec4)> dot4("dot");
    Function<vec4(bool,vec4,vec4)> if4_("if");
    Function<vec4(float,float,float,float)> vec4_("vec4");
}
