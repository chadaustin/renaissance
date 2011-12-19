#pragma once

#include <string>
#include <ren/Types.h>

namespace ren {

    class VertexShader2 {
    public:
        struct Outputs {
            vec4 position;
        };

        Outputs output;

        std::string generateGLSL() const {
            return "";
        };
    };

}
