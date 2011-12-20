#pragma once

#include <vector>
#include <memory>
#include <ren/ID.h>

#define REN_PTR(C) typedef std::shared_ptr<class C> C##Ptr

namespace ren {

    enum Type {
        VEC4,
        INT,
        IVEC4,
        MAT4,
        ARRAY,
    };

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

    class Add : public Expression {
    public:
        Add(const ExpressionPtr& left, const ExpressionPtr& right)
            : left(left)
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

    class IntLiteral : public Expression {
    public:
        explicit IntLiteral(int i)
        : i(i)
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
        : f(f)
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
            : base(base)
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
            : base(base)
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
        Function(const char* name, const ExpressionPtr& a0, const ExpressionPtr& a1)
            : name(name)
        {
            args.push_back(a0);
            args.push_back(a1);
        }

        Function(const char* name, const ExpressionPtr& a0, const ExpressionPtr& a1, const ExpressionPtr& a2, const ExpressionPtr& a3)
            : name(name)
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

    class ExpressionHandle {
    public:
        ExpressionHandle() {
        }

        explicit ExpressionHandle(const ExpressionPtr& expression)
            : expression(expression)
        {}

        ExpressionPtr expression;
    };
    
    class float_ : public ExpressionHandle {
    public:
        float_() {
        }

        float_(float f)
        : ExpressionHandle(std::make_shared<FloatLiteral>(f))
        {}

        float_(const ExpressionPtr& expression)
            : ExpressionHandle(expression)
        {}
    };

    class int_ : public ExpressionHandle {
    public:
        int_() {
        }

        int_(const ExpressionPtr& expression)
            : ExpressionHandle(expression)
        {}
    };

    class ivec4 : public ExpressionHandle {
    public:
        static Type type() { return IVEC4; }

        ivec4() {
        }

        int_ x() const {
            return int_(std::make_shared<Swizzle>(expression, "x"));
        }

        int_ y() const {
            return int_(std::make_shared<Swizzle>(expression, "y"));
        }

        int_ z() const {
            return int_(std::make_shared<Swizzle>(expression, "z"));
        }

        int_ w() const {
            return int_(std::make_shared<Swizzle>(expression, "w"));
        }

        explicit ivec4(const ExpressionPtr& source)
            : ExpressionHandle(source)
        {}
    };

    class vec4 : public ExpressionHandle {
    public:
        static Type type() { return VEC4; }

        vec4() {
        }

        vec4(float_ x, float_ y, float_ z, float_ w)
            : ExpressionHandle(std::make_shared<Function>("vec4", x.expression, y.expression, z.expression, w.expression))
        {}

        explicit vec4(const ExpressionPtr& source)
            : ExpressionHandle(source)
        {}

        float_ x() const {
            return float_(std::make_shared<Swizzle>(expression, "x"));
        }

        float_ y() const {
            return float_(std::make_shared<Swizzle>(expression, "y"));
        }

        float_ z() const {
            return float_(std::make_shared<Swizzle>(expression, "z"));
        }

        float_ w() const {
            return float_(std::make_shared<Swizzle>(expression, "w"));
        }

        vec4& operator+=(const vec4& right) {
            expression = std::make_shared<Add>(expression, right.expression);
            return *this;
        }
    };

    class mat4 : public ExpressionHandle {
    public:
        static Type type() { return MAT4; }

        explicit mat4(const ExpressionPtr& source)
            : ExpressionHandle(source)
        {}
    };

    vec4 operator*(const vec4& left, const float_& right) {
        return vec4(std::make_shared<Multiply>(left.expression, right.expression));
    }

    mat4 operator*(const mat4& left, const mat4& right) {
        return mat4(std::make_shared<Multiply>(left.expression, right.expression));
    }

    vec4 operator*(const mat4& m, const vec4& v) {
        return vec4(std::make_shared<Multiply>(m.expression, v.expression));
    }

    ivec4 operator*(int i, const ivec4& v) {
        return ivec4(std::make_shared<Multiply>(std::make_shared<IntLiteral>(i), v.expression));
    }

    int_ operator+(const int_& left, int right) {
        return int_(std::make_shared<Add>(left.expression, std::make_shared<IntLiteral>(right)));
    }

    vec4 operator+(const vec4& left, const vec4& right) {
        return vec4(std::make_shared<Add>(left.expression, right.expression));
    }

    float_ dot(const vec4& left, const vec4& right) {
        return float_(std::make_shared<Function>("dot", left.expression, right.expression));
    }

    template<typename T, size_t Length>
    class array : public ExpressionHandle {
    public:
        static Type type() { return ARRAY; } // TODO: wrong

        array() {
        }

        array(const ExpressionPtr& source)
            : ExpressionHandle(source)
        {}

        T operator[](const int_& index) const {
            return T(std::make_shared<Index>(expression, index.expression));
        }
    };

    template<typename T, typename NodeType>
    class Input {
    public:
        template<typename Proxy=T>
        auto operator[](const int_& i) const -> decltype(std::declval<Proxy>()[i]) {
            return Proxy(*this)[i];
        }

        operator T() const {
            return T(std::make_shared<NodeType>(id, T::type()));
        }

    private:
        ID id;
    };

    template<typename T>
    class attribute : public Input<T, AttributeExpression> {
    };

    template<typename T>
    class uniform : public Input<T, UniformExpression> {
    };
};
