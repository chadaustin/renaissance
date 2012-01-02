#pragma once

namespace ren {

    class AbstractValue {
    public:
        virtual ~AbstractValue() {}
    };
    REN_PTR(AbstractValue);

    template<typename T>
    class DataValue : public AbstractValue {
    public:
        DataValue()
            : value()
        {}

        explicit DataValue(const T& v)
            : value(v)
        {}

        T value;
    };

}
