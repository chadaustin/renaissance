#pragma once

#include <string.h>
#include <vector>
#include <memory>
#include <ren/ID.h>
#include <ren/Type.h>
#include <ren/AST.h>

namespace ren {
    class ExpressionHandle {
    public:
        ExpressionHandle() {
        }

        explicit ExpressionHandle(const ExpressionPtr& expression)
            : expression(expression)
        {}

        ExpressionPtr expression;
    };

    class bool_ : public ExpressionHandle {
    public:
        static Type type() { return BOOL; }

        bool_() {
        }

        bool_(const ExpressionPtr& expression)
            : ExpressionHandle(expression)
        {}
    };
    
    class int_ : public ExpressionHandle {
    public:
        static Type type() { return INT; }

        int_() {
        }

        int_(int i)
            : ExpressionHandle(std::make_shared<InputExpression>(ID(), type(), CONSTANT, std::make_shared<DataValue<int>>(i)))
        {}

        int_(const ExpressionPtr& expression)
            : ExpressionHandle(expression)
        {}
    };

    class float_ : public ExpressionHandle {
    public:
        static Type type() { return FLOAT; }

        float_() {
        }

        float_(float f)
            : ExpressionHandle(std::make_shared<InputExpression>(ID(), type(), CONSTANT, std::make_shared<DataValue<float>>(f)))
        {}

        float_(const ExpressionPtr& expression)
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
            : ExpressionHandle(std::make_shared<FunctionCall>(&vec4_, x.expression, y.expression, z.expression, w.expression))
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
            expression = std::make_shared<FunctionCall>(&vec4_plus, expression, right.expression);
            return *this;
        }

        vec4& operator*=(const vec4& right) {
            expression = std::make_shared<FunctionCall>(&vec4_times_vec4, expression, right.expression);
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
        return vec4(std::make_shared<FunctionCall>(&vec4_times_float, left.expression, right.expression));
    }

    mat4 operator*(const mat4& left, const mat4& right) {
        return mat4(std::make_shared<FunctionCall>(&mat4_times_mat4, left.expression, right.expression));
    }

    vec4 operator*(const mat4& m, const vec4& v) {
        return vec4(std::make_shared<FunctionCall>(&mat4_times_vec4, m.expression, v.expression));
    }

    ivec4 operator*(int i, const ivec4& v) {
        return ivec4(std::make_shared<FunctionCall>(&int_times_ivec4, int_(i).expression, v.expression));
    }

    int_ operator+(const int_& left, int right) {
        return int_(std::make_shared<FunctionCall>(&int_plus, left.expression, int_(right).expression));
    }

    vec4 operator+(const vec4& left, const vec4& right) {
        return vec4(std::make_shared<FunctionCall>(&vec4_plus, left.expression, right.expression));
    }

    float_ dot(const vec4& left, const vec4& right) {
        return float_(std::make_shared<FunctionCall>(&dot4, left.expression, right.expression));
    }

    vec4 if_(const bool_& condition, const vec4& left, const vec4& right) {
        return vec4(std::make_shared<FunctionCall>(&if4_, condition.expression, left.expression, right.expression));
    }

    template<typename T, size_t Length>
    class array : public ExpressionHandle {
    public:
        static Type type() { return Type::array(T::type(), Length); }

        array() {
        }

        array(const ExpressionPtr& source)
            : ExpressionHandle(source)
        {}

        T operator[](const int_& index) const {
            return T(std::make_shared<Index>(expression, index.expression));
        }
    };

    template<typename T, Frequency frequency>
    class Input {
    public:
        typedef typename NativeToRen<T>::type ren_type;

        template<typename Proxy=ren_type>
        auto operator[](const int_& i) const -> decltype(std::declval<Proxy>()[i]) {
            return Proxy(*this)[i];
        }

        operator ren_type() const {
            return ren_type(std::make_shared<InputExpression>(id, ren_type::type(), frequency, value));
        }

    protected:
        std::shared_ptr<DataValue<T>> value;

    private:
        ID id;
    };

    template<typename T>
    class constant : public Input<T, CONSTANT> {
    public:
        using Input<T, CONSTANT>::value;

        constant()
            : Input<T, CONSTANT>()
        {
            value.reset(new DataValue<T>());
        }

        constant& operator=(const T& v) {
            value->value = v;
            return *this;
        }
    };

    template<typename T>
    class uniform : public Input<T, UNIFORM> {
    };

    template<typename T>
    class attribute : public Input<T, ATTRIBUTE> {
    };

};
