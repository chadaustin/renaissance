#pragma once

#include <ren/API.h>

namespace ren {

    class VertexShader2 {
    public:
        vec4 position;
        vec4 color;
        vec4 secondaryColor;
        vec4 texCoord[8];
    };

}
