#ifndef REN_TYPES_H
#define REN_TYPES_H


#include <string>

namespace ren {
    using std::string;

    enum Type {
        FLOAT,
        VEC2,
        VEC3,
        VEC4,

        INT,
        VEC2I,
        VEC3I,
        VEC4I,

        BOOL,
        VEC2B,
        VEC3B,
        VEC4B,

        MAT2,
        MAT3,
        MAT4,

        SAMPLER1D,
        SAMPLER2D,
        SAMPLER3D,
        SAMPLERCUBE,
        SAMPLER1DSHADOW,
        SAMPLER2DSHADOW,
    };
}


#endif
