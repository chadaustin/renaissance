#pragma once

#include <memory>
#include <ren/ID.h>

#define REN_PTR(C) typedef std::shared_ptr<class C> C##Ptr

namespace ren {

    class Expression {
    public:
    };
    REN_PTR(Expression);

    class Multiply : public Expression {
    public:
        Multiply(const ExpressionPtr& left, const ExpressionPtr& right)
            : left(left)
            , right(right)
        {}
        ExpressionPtr left;
        ExpressionPtr right;

    private:
        Multiply() = delete;
    };

    class AttributeExpression : public Expression {
    public:
        AttributeExpression(const ID& id)
            : id(id)
        {}

        const ID id;
    };

    class UniformExpression : public Expression {
    public:
        UniformExpression(const ID& id)
            : id(id)
        {}
        const ID id;
    };

    class ExpressionHandle {
    public:
        ExpressionHandle() {
        }

        explicit ExpressionHandle(const ExpressionPtr& expression)
            : expression(expression)
        {}

        ExpressionPtr expression;
    };

    class mat4 : public ExpressionHandle {
    public:
        explicit mat4(const ExpressionPtr& source)
            : ExpressionHandle(source)
        {}
    };

    mat4 operator*(const mat4& left, const mat4& right) {
        return mat4(std::make_shared<Multiply>(left.expression, right.expression));
    }

    class vec4 : public ExpressionHandle {
    public:
        vec4() {
        }

        explicit vec4(const ExpressionPtr& source)
            : ExpressionHandle(source)
        {}
    };

    vec4 operator*(const mat4& m, const vec4& v) {
        return vec4(std::make_shared<Multiply>(m.expression, v.expression));
    }

    template<typename T>
    class attribute {
    public:
        operator T() const {
            return T(std::make_shared<AttributeExpression>(id));
        }

    private:
        ID id;
    };

    template<typename T>
    class uniform {
    public:
        ID id;

    private:
        operator T() const {
            return T(std::make_shared<UniformExpression>(id));
        }
    };
};
