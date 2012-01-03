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
        typedef std::pair<Type, std::string> Decl;
        std::map<ID, std::pair<Decl, AbstractValuePtr>> constants;
        std::map<ID, std::pair<Decl, AbstractValuePtr>> uniforms;
        std::map<ID, std::pair<Decl, AbstractValuePtr>> attributes;
        std::vector<std::pair<Decl, std::string>> locals;
        std::map<std::string, ExpressionPtr> outputExpressions;
        std::map<std::string, std::string> outputs;
        std::map<ExpressionPtr, std::string> expressionNames;

        GLSLGenerator()
            : constantCount(0)
            , uniformCount(0)
            , attributeCount(0)
            , localCount(0)
        {}

        void addOutput(const std::string& name, const ExpressionHandle& output) {
            if (output.expression) {
                outputExpressions[name] = output.expression;
            }
        }

        void compile() {
            for (auto i = outputExpressions.begin(); i != outputExpressions.end(); ++i) {
                generateOutput(i->first, i->second);
            }
        }

    private:
        void generateOutput(const std::string& outputName, const ExpressionPtr& expression) {
            walk(expression);
            outputs[outputName] = popTop();
        }

        void walk(const ExpressionPtr& p) {
            if (expressionNames.count(p)) {
                stack.push(expressionNames[p]);
            } else if (p->operands.empty()) {
                p->walk(*this);
                expressionNames[p] = getTop();
            } else {
                for (auto o = p->operands.begin(); o != p->operands.end(); ++o) {
                    walk(*o);
                }
                p->walk(*this);
                std::string result = popTop();
                std::string name = allocateLocalName();
                locals.push_back(std::make_pair(Decl(p->type, name), result));
                expressionNames[p] = name;
                stack.push(name);
            }
        }

    public:
        std::string decl(const Decl& d) {
            const auto& type = d.first;
            const auto& name = d.second;
            char array_suffix[80] = {0};
            if (type.is_array) {
                sprintf(array_suffix, "[%u]", type.array_length);
            }
            return nameOf(type) + " " + name + array_suffix;
        }

        // ExpressionWalker implementation:

        void pushInput(const ID& id, Frequency frequency, Type type, AbstractValuePtr value) {
            std::map<ID, std::pair<Decl, AbstractValuePtr>>* p;
            auto a = &GLSLGenerator::allocateConstantName;
            switch (frequency) {
                case CONSTANT: p = &constants; a = &GLSLGenerator::allocateConstantName; break;
                case UNIFORM: p = &uniforms; a = &GLSLGenerator::allocateUniformName; break;
                case ATTRIBUTE: p = &attributes; a = &GLSLGenerator::allocateAttributeName; break;
            }
            if (!p->count(id)) {
                (*p)[id] = std::make_pair(Decl(type, (this->*a)()), value);
            }
            stack.push((*p)[id].first.second);
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

        void swizzle(const char* swizzle) {
            std::string base(popTop());
            stack.push(base + "." + swizzle);
        }

        void index() {
            std::string index(popTop());
            std::string base(popTop());
            stack.push(base + "[" + index + "]");
        }

        void apply(const FunctionBase* function, unsigned argCount) {
            std::vector<std::string> args;
            while (argCount--) {
                args.push_back(popTop());
            }
            if (function->is_operator) {
                verify(2 == function->signature.arguments.size());
                verify(2 == args.size());
                stack.push(std::string("(") + args[1] + " " + function->glsl_name + " " + args[0] + ")");
            } else {
                std::string n(function->glsl_name);
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
        }

    private:
        std::string getTop() {
            return stack.top();
        }
        std::string popTop() {
            std::string rv(stack.top());
            stack.pop();
            return rv;
        };

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

        std::string allocateConstantName() {
            char p[80];
            sprintf(p, "c%u", constantCount++);
            return p;
        }

        std::string allocateUniformName() {
            char p[80];
            sprintf(p, "u%u", uniformCount++);
            return p;
        }

        std::string allocateAttributeName() {
            char p[80];
            sprintf(p, "a%u", attributeCount++);
            return p;
        }

        std::string allocateLocalName() {
            char p[80];
            sprintf(p, "t%u", localCount++);
            return p;
        }

        std::stack<std::string> stack;
        unsigned constantCount;
        unsigned uniformCount;
        unsigned attributeCount;
        unsigned localCount;
    };
    
    inline std::string generateGLSL(const VertexShader2& vertexShader) {
        GLSLGenerator g;

        std::ostringstream os;

        g.addOutput("gl_Position", vertexShader.position);
        g.addOutput("gl_FrontColor", vertexShader.color);
        g.addOutput("gl_FrontSecondaryColor", vertexShader.secondaryColor);
        for (int i = 0; i < 8; ++i) {
            char n[] = "gl_TexCoord[_]";
            n[12] = '0' + i;
            g.addOutput(n, vertexShader.texCoord[i]);
        }

        g.compile();

        for (auto i = g.constants.begin(); i != g.constants.end(); ++i) {
            os << "const " << g.decl(i->second.first) << " = " << i->second.second->asString() << ";\n";
        }

        for (auto i = g.uniforms.begin(); i != g.uniforms.end(); ++i) {
            os << "uniform " << g.decl(i->second.first) << ";\n";
        }

        for (auto i = g.attributes.begin(); i != g.attributes.end(); ++i) {
            os << "attribute " << g.decl(i->second.first) << ";\n";
        }

        os << "void main() {\n";
        for (auto i = g.locals.begin(); i != g.locals.end(); ++i) {
            os << "    " << g.decl(i->first) << " = " << i->second << ";\n";
        }
        for (auto i = g.outputs.begin(); i != g.outputs.end(); ++i) {
            os << "    " << i->first << " = " << i->second << ";\n";
        }
        os << "}\n";
        return os.str();
    }

}
