#pragma once

#include <stdio.h>
#include <map>
#include <sstream>
#include <string>
#include <stack>
#include <vector>
#include <ren/Types.h>

namespace ren {

    class GLSLGenerator : public ExpressionWalker {
    public:
        std::map<ID, std::pair<std::string, std::string>> attributes;
        std::map<ID, std::pair<std::string, std::string>> uniforms;

        GLSLGenerator()
            : attributeCount(0)
            , uniformCount(0)
        {}

        void pushAttribute(const ID& id, Type type) {
            if (!attributes.count(id)) {
                attributes[id] = std::make_pair(nameOf(type), allocateAttributeName());
            }
            stack.push(attributes[id].second);
        }

        void pushUniform(const ID& id, Type type) {
            if (!uniforms.count(id)) {
                uniforms[id] = std::make_pair(nameOf(type), allocateUniformName());
            }
            stack.push(uniforms[id].second);
        }

        void pushInt(int i) {
            char p[80];
            sprintf(p, "%d", i);
            stack.push(p);
        }

        void pushFloat(float f) {
            char p[80];
            sprintf(p, "%f", f);
            stack.push(p);
        }

        void multiply() {
            std::string right(popTop());
            std::string left(popTop());
            stack.push("(" + left + " * " + right + ")");
        }

        void add() {
            std::string right(popTop());
            std::string left(popTop());
            stack.push("(" + left + " + " + right + ")");
        }

        void swizzle(const char* swizzle) {
            std::string base(popTop());
            stack.push(base + "." + swizzle);
        }

        void index() {
            std::string index(popTop());
            std::string base(popTop());
            stack.push(base + "[" + index + "]");
        }

        void call(const char* name, unsigned argCount) {
            std::vector<std::string> args;
            while (argCount--) {
                args.push_back(popTop());
            }
            std::string n(name);
            n += "(";
            bool first = true;
            while (!args.empty()) {
                if (!first) {
                    n += ", ";
                }
                first = false;
                n += args.back();
                args.pop_back();
            }

            n += ")";
            stack.push(n);
        }

        std::string popTop() {
            std::string rv(stack.top());
            stack.pop();
            return rv;
        };

    private:
        std::string nameOf(Type type) {
            switch (type) {
                case VEC4: return "vec4";
                case IVEC4: return "ivec4";
                case INT: return "int";
                case MAT4: return "mat4";
                case ARRAY: return "array"; // wrong
            }
            return "???";
        }

        std::string allocateAttributeName() {
            char p[80];
            sprintf(p, "a%u", attributeCount++);
            return p;
        }

        std::string allocateUniformName() {
            char p[80];
            sprintf(p, "u%u", uniformCount++);
            return p;
        }

        std::stack<std::string> stack;
        unsigned attributeCount;
        unsigned uniformCount;
    };
    
    class VertexShader2 {
    public:
        struct Outputs {
            vec4 position;
        };

        Outputs output;

        std::string generateGLSL() const {
            GLSLGenerator g;

            std::ostringstream os;
            
            output.position.expression->walk(g);

            for (auto i = g.uniforms.begin(); i != g.uniforms.end(); ++i) {
                os << "uniform " << i->second.first << " " << i->second.second << ";\n";
            }

            for (auto i = g.attributes.begin(); i != g.attributes.end(); ++i) {
                os << "attribute " << i->second.first << " " << i->second.second << ";\n";
            }

            os
               << "void main() {\n"
               << "    gl_Position = " << g.popTop() << ";\n"
               << "}\n";
            return os.str();
        };
    };

}
