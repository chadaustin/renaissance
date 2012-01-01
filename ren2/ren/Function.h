#pragma once

#include <ren/Signature.h>
#include <ren/Type.h>

namespace ren {

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
            : FunctionBase(name, getSignature<SignatureT>())
        {}
    };

    Function<float(vec3,vec3)> dot3("dot");
    Function<float(vec4,vec4)> dot4("dot");
    Function<vec4(bool,vec4,vec4)> if4_("if");
    Function<vec4(float,float,float,float)> vec4_("vec4");
}
