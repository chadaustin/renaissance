#pragma once

#include <string.h>
#include <ren/Global.h>
#include <ren/ID.h>
#include <ren/Type.h>
#include <ren/Function.h>
#include <ren/Value.h>

namespace ren {

    REN_PTR(Expression);

    class ExpressionWalker {
    public:
        virtual void walk(const ExpressionPtr& p) = 0;
        virtual void pushInput(const ID& id, Frequency frequency, Type type, AbstractValuePtr value) = 0;
        virtual void swizzle(const char* swizzle) = 0;
        virtual void apply(const FunctionBase* function, unsigned argCount) = 0;
    };

    inline Frequency getMaximumFrequency(const std::vector<ExpressionPtr>& operands);

    class Expression {
    public:
        Expression(Type type, Frequency frequency, const AbstractValuePtr& value)
            : type(type)
            , frequency(frequency)
            , value(value)
        {}

        Expression(Type type, const std::vector<ExpressionPtr>& operands)
            : type(type)
            , frequency(getMaximumFrequency(operands))
            , operands(operands)
        {}

        template<typename T>
        T evaluate() const {
            verify(type == getType<T>());
            verify(frequency == CONSTANT);
            auto p = checked_cast<DataValue<T>*>(value.get());
            verify(p);
            return p->value;
        }

        const Type type;
        const Frequency frequency;
        const AbstractValuePtr value;
        const std::vector<ExpressionPtr> operands;

        virtual void walk(ExpressionWalker& w) = 0;
    };

    inline Frequency getMaximumFrequency(const std::vector<ExpressionPtr>& operands) {
        verify(!operands.empty());
        Frequency result = operands[0]->frequency;
        for (size_t i = 1; i < operands.size(); ++i) {
            result = std::max(result, operands[i]->frequency);
        }
        return result;
    }

    class InputExpression : public Expression {
    public:
        InputExpression(const ID& id, Type type, Frequency frequency, const AbstractValuePtr& value)
            : Expression(type, frequency, value)
            , id(id)
        {}

        const ID id;

        void walk(ExpressionWalker& w) {
            verify(operands.empty());
            w.pushInput(id, frequency, type, value);
        }
    };

    class Swizzle : public Expression {
    public:
        Swizzle(const ExpressionPtr& base, const char* swizzle)
            : Expression(Type::vector(base->type.element_type, strlen(swizzle)), {base})
            , swizzle(swizzle)
        {}

        void walk(ExpressionWalker& w) {
            for (auto o = operands.begin(); o != operands.end(); ++o) {
                w.walk(*o);
            }
            w.swizzle(swizzle);
        }

        const char* const swizzle;
    };

    class FunctionCall : public Expression {
    public:
        FunctionCall(const FunctionBase* fn, const ExpressionPtr& a0, const ExpressionPtr& a1)
            : Expression(fn->signature.result, {a0, a1})
            , function(fn)
        {
        }

        FunctionCall(const FunctionBase* fn, const ExpressionPtr& a0, const ExpressionPtr& a1, const ExpressionPtr& a2)
            : Expression(fn->signature.result, {a0, a1, a2})
            , function(fn)
        {
        }

        FunctionCall(const FunctionBase* fn, const ExpressionPtr& a0, const ExpressionPtr& a1, const ExpressionPtr& a2, const ExpressionPtr& a3)
            : Expression(fn->signature.result, {a0, a1, a2, a3})
            , function(fn)
        {
        }

        void walk(ExpressionWalker& w) {
            for (auto o = operands.begin(); o != operands.end(); ++o) {
                w.walk(*o);
            }
#if 0
            // HACK: apply conditionals here
            if (function == &if4_) {
                verify(3 == operands.size());
                if (operands[0]->frequency == CONSTANT) {
                    if (operands[0]->evaluate<bool>()) {
                        return operands[1]->walk(w);
                    } else {
                        return operands[2]->walk(w);
                    }
                }
            }
#endif
            w.apply(function, operands.size());
        }

        const FunctionBase* const function;
    };

}

