#pragma once

#include <map>
#include <sstream>
#include <string>
#include <ren/Types.h>

namespace ren {

    struct Uniform {
        std::string type;
    };

    struct UniformSet {
        UniformSet()
            : current(0)
        {}

        unsigned current;
        std::map<std::string, Uniform> uniforms;
    };

    struct AttributeSet {
        AttributeSet()
            : current(0)
        {}

        unsigned current;
        std::map<std::string, std::string> attributes;
    };
    
    class VertexShader2 {
    public:
        struct Outputs {
            vec4 position;
        };

        Outputs output;

        std::string generateGLSL() const {
            std::ostringstream os;
            UniformSet uniforms;
            AttributeSet attributes;

            for (auto i = uniforms.uniforms.begin(); i != uniforms.uniforms.end(); ++i) {
                os << "uniform " << i->second.type << " " << i->first << ";\n";
            }

            for (auto i = attributes.attributes.begin(); i != attributes.attributes.end(); ++i) {
                os << "attribute " << i->second << " " << i->first << ";\n";
            }

            scan(uniforms, attributes, output.position);

            os
               << "void main() {\n"
               << "gl_Position = a0;\n"
               << "}\n";
            return os.str();
        };

        void scan(UniformSet& uniforms, AttributeSet& attributes, const vec4& output) const {
            
        }
    };

}
