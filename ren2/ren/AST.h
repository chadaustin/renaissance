#pragma once

#include <string.h>
#include <ren/Global.h>
#include <ren/ID.h>
#include <ren/Type.h>

namespace ren {

    class ExpressionWalker {
    public:
        virtual void pushAttribute(const ID& id, Type type) = 0;
        virtual void pushUniform(const ID& id, Type type) = 0;
        virtual void pushInt(int i) = 0;
        virtual void pushFloat(float f) = 0;
        virtual void multiply() = 0;
        virtual void add() = 0;
        virtual void swizzle(const char* swizzle) = 0;
        virtual void index() = 0;
        virtual void call(const char* name, unsigned argCount) = 0;
    };

    class Expression {
    public:
        Expression(Type type)
            : type(type)
        {}

        const Type type;

        virtual void walk(ExpressionWalker& w) = 0;
    };
    REN_PTR(Expression);

    class Multiply : public Expression {
    public:
        Multiply(Type type, const ExpressionPtr& left, const ExpressionPtr& right)
            : Expression(type)
            , left(left)
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

    class Add : public Expression {
    public:
        Add(Type type, const ExpressionPtr& left, const ExpressionPtr& right)
            : Expression(type)
            , left(left)
            , right(right)
        {}

        ExpressionPtr left;
        ExpressionPtr right;

        void walk(ExpressionWalker& w) {
            left->walk(w);
            right->walk(w);
            w.add();
        }

    private:
        Add() = delete;
    };

    class AttributeExpression : public Expression {
    public:
        AttributeExpression(const ID& id, Type type)
            : Expression(type)
            , id(id)
        {}

        const ID id;

        void walk(ExpressionWalker& w) {
            w.pushAttribute(id, type);
        }
    };

    class UniformExpression : public Expression {
    public:
        UniformExpression(const ID& id, Type type)
            : Expression(type)
            , id(id)
        {}

        const ID id;

        void walk(ExpressionWalker& w) {
            w.pushUniform(id, type);
        }
    };

    class IntLiteral : public Expression {
    public:
        explicit IntLiteral(int i)
            : Expression(Type::scalar(Type::INT))
            , i(i)
        {}

        void walk(ExpressionWalker& w) {
            w.pushInt(i);
        }

    private:
        int i;
    };

    class FloatLiteral : public Expression {
    public:
        explicit FloatLiteral(float f)
            : Expression(Type::scalar(Type::FLOAT))
            , f(f)
        {}

        void walk(ExpressionWalker& w) {
            w.pushFloat(f);
        }

    private:
        float f;
    };

    class Swizzle : public Expression {
    public:
        Swizzle(const ExpressionPtr& base, const char* swizzle)
            : Expression(Type::vector(base->type.element_type, strlen(swizzle)))
            , base(base)
            , swizzle(swizzle)
        {}

        void walk(ExpressionWalker& w) {
            base->walk(w);
            w.swizzle(swizzle);
        }

        const ExpressionPtr base;
        const char* const swizzle;
    };

    class Index : public Expression {
    public:
        Index(const ExpressionPtr& base, const ExpressionPtr& index)
            : Expression(base->type.withoutArray())
            , base(base)
            , index(index)
        {}

        void walk(ExpressionWalker& w) {
            base->walk(w);
            index->walk(w);
            w.index();
        }

        const ExpressionPtr base;
        const ExpressionPtr index;
    };

    class Function : public Expression {
    public:
        Function(Type type, const char* name, const ExpressionPtr& a0, const ExpressionPtr& a1)
            : Expression(type)
            , name(name)
        {
            args.push_back(a0);
            args.push_back(a1);
        }

        Function(Type type, const char* name, const ExpressionPtr& a0, const ExpressionPtr& a1, const ExpressionPtr& a2, const ExpressionPtr& a3)
            : Expression(type)
            , name(name)
        {
            args.push_back(a0);
            args.push_back(a1);
            args.push_back(a2);
            args.push_back(a3);
        }

        void walk(ExpressionWalker& w) {
            for (auto i = args.begin(); i != args.end(); ++i) {
                (*i)->walk(w);
            }
            w.call(name, args.size());
        }

        const char* const name;
        std::vector<ExpressionPtr> args;
    };

}

