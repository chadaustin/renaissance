#pragma once

#include <stdio.h>
#include <map>
#include <sstream>
#include <string>
#include <stack>
#include <vector>
#include <ren/AST.h>
#include <ren/VertexShader.h>

namespace ren {

    class GLSLGenerator : public ExpressionWalker {
    public:
        std::map<ID, std::pair<Type, std::string>> attributes;
        std::map<ID, std::pair<Type, std::string>> uniforms;

        GLSLGenerator()
            : attributeCount(0)
            , uniformCount(0)
        {}

        void pushAttribute(const ID& id, Type type) {
            if (!attributes.count(id)) {
                attributes[id] = std::make_pair(type, allocateAttributeName());
            }
            stack.push(attributes[id].second);
        }

        void pushUniform(const ID& id, Type type) {
            if (!uniforms.count(id)) {
                uniforms[id] = std::make_pair(type, allocateUniformName());
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

        std::string decl(Type type, const std::string& name) {
            char array_suffix[80] = {0};
            if (type.is_array) {
                sprintf(array_suffix, "[%u]", type.array_length);
            }
            return nameOf(type) + " " + name + array_suffix;
        }

    private:
        std::string nameOf(Type type) {
            if (type.columns) {
                // matrix
                // TODO: verify(Type::FLOAT == type.element_type);
                char c[2] = { char('1' + type.columns) };
                if (type.columns == type.rows) {
                    return std::string("mat") + c;
                } else {
                    char r[2] = { char('1' + type.rows) };
                    return std::string("mat") + c + "x" + r;
                }
            } else if (type.rows) {
                // vector
                std::string prefix;
                if (type.element_type == Type::BOOL) {
                    prefix = "b";
                } else if (type.element_type == Type::INT) {
                    prefix = "i";
                }
                char d[2] = { char('1' + type.rows) };
                return prefix + "vec" + d;
            } else {
                // scalar
                switch (type.element_type) {
                    case Type::BOOL: return "bool";
                    case Type::INT: return "int";
                    case Type::FLOAT: return "float";
                    default: return "???";
                }
            }
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
    
    inline std::string generateGLSL(const VertexShader2& vertexShader) {
        GLSLGenerator g;

        std::ostringstream os;
            
        vertexShader.position.expression->walk(g);

        for (auto i = g.uniforms.begin(); i != g.uniforms.end(); ++i) {
            os << "uniform " << g.decl(i->second.first, i->second.second) << ";\n";
        }

        for (auto i = g.attributes.begin(); i != g.attributes.end(); ++i) {
            os << "attribute " << g.decl(i->second.first, i->second.second) << ";\n";
        }

        os
            << "void main() {\n"
            << "    gl_Position = " << g.popTop() << ";\n"
            << "}\n";
        return os.str();
    }

}
