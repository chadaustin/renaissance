#pragma once

#include <ren/Signature.h>
#include <ren/Type.h>

namespace ren {

    enum FunctionType {
        FUNCTION,
        OPERATOR,
        INDEX,
    };

    struct FunctionBase {
        FunctionBase(FunctionType function_type, const char* glsl_name, const Signature& signature)
            : function_type(function_type)
            , glsl_name(glsl_name)
            , signature(signature)
        {}

        const FunctionType function_type;
        const char* const glsl_name;
        const Signature signature;
    };

    template<typename SignatureT>
    struct Function : FunctionBase {
        Function(const char* name)
            : FunctionBase(FUNCTION, name, getSignature<SignatureT>())
        {}
    };

    template<typename SignatureT>
    struct Operator : FunctionBase {
        Operator(const char* name)
            : FunctionBase(OPERATOR, name, getSignature<SignatureT>())
        {}
    };

    template<typename SignatureT>
    struct Index : FunctionBase {
        Index()
            : FunctionBase(INDEX, "???", getSignature<SignatureT>())
        {}
    };

    Function<float(vec3,vec3)> dot3("dot");
    Function<float(vec4,vec4)> dot4("dot");
    Function<vec4(bool,vec4,vec4)> if4_("if");
    Function<vec4(float,float,float,float)> vec4_("vec4");
    Operator<int(int,int)> int_plus("+");
    Operator<vec4(vec4,vec4)> vec4_plus("+");
    Operator<vec4(vec4,vec4)> vec4_times_vec4("*");
    Operator<vec4(vec4,float)> vec4_times_float("*");
    Operator<mat4(mat4,mat4)> mat4_times_mat4("*");
    Operator<vec4(mat4,vec4)> mat4_times_vec4("*");
    Operator<ivec4(int,ivec4)> int_times_ivec4("*");
    Index<vec4(vec4[],int)> vec4_index;
}
