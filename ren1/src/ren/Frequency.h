#ifndef REN_FREQUENCY_H
#define REN_FREQUENCY_H


#include <iostream>


namespace ren {

    enum Frequency {
        CONSTANT,
        UNIFORM,
        VERTEX,
        FRAGMENT,
    };


    inline std::ostream& operator<<(std::ostream& os, Frequency f) {
        switch (f) {
            case CONSTANT: return os << "constant";
            case UNIFORM:  return os << "uniform";
            case VERTEX:   return os << "vertex";
            case FRAGMENT: return os << "fragment";
            default: assert(!"Invalid frequency"); return os;
        }
    }

}


#endif
