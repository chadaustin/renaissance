#pragma once

#include <ren/Signature.h>
#include <ren/Type.h>

namespace ren {

    struct FunctionBase {
        FunctionBase(bool is_operator, const char* glsl_name, const Signature& signature)
            : is_operator(is_operator)
            , glsl_name(glsl_name)
            , signature(signature)
        {}

        const bool is_operator;
        const char* const glsl_name;
        const Signature signature;
    };

    template<typename SignatureT>
    struct Function : FunctionBase {
        Function(const char* name)
            : FunctionBase(false, name, getSignature<SignatureT>())
        {}
    };

    template<typename SignatureT>
    struct Operator : FunctionBase {
        Operator(const char* name)
            : FunctionBase(true, name, getSignature<SignatureT>())
        {}
    };

    Function<float(vec3,vec3)> dot3("dot");
    Function<float(vec4,vec4)> dot4("dot");
    Function<vec4(bool,vec4,vec4)> if4_("if");
    Function<vec4(float,float,float,float)> vec4_("vec4");
    Operator<int(int,int)> int_plus("+");
    Operator<vec4(vec4,vec4)> vec4_plus("+");
}
