#pragma once

#include <memory>
#include <ren/ID.h>

#define REN_PTR(C) typedef std::shared_ptr<class C> C##Ptr

namespace ren {

    enum Type {
        VEC4,
        MAT4,
    };

    class ExpressionWalker {
    public:
        virtual void pushAttribute(const ID& id, Type type) = 0;
        virtual void pushUniform(const ID& id, Type type) = 0;
        virtual void multiply() = 0;
    };

    class Expression {
    public:
        virtual void walk(ExpressionWalker& w) = 0;
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

        void walk(ExpressionWalker& w) {
            left->walk(w);
            right->walk(w);
            w.multiply();
        }

    private:
        Multiply() = delete;
    };

    class AttributeExpression : public Expression {
    public:
        AttributeExpression(const ID& id, Type type)
            : id(id)
            , type(type)
        {}

        const ID id;
        const Type type;

        void walk(ExpressionWalker& w) {
            w.pushAttribute(id, type);
        }
    };

    class UniformExpression : public Expression {
    public:
        UniformExpression(const ID& id, Type type)
            : id(id)
            , type(type)
        {}

        const ID id;
        const Type type;

        void walk(ExpressionWalker& w) {
            w.pushUniform(id, type);
        }
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
        static Type type() { return MAT4; }

        explicit mat4(const ExpressionPtr& source)
            : ExpressionHandle(source)
        {}
    };

    mat4 operator*(const mat4& left, const mat4& right) {
        return mat4(std::make_shared<Multiply>(left.expression, right.expression));
    }

    class vec4 : public ExpressionHandle {
    public:
        static Type type() { return VEC4; }

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
            return T(std::make_shared<AttributeExpression>(id, T::type()));
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
            return T(std::make_shared<UniformExpression>(id, T::type()));
        }
    };
};
