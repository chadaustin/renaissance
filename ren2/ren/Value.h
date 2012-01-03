#pragma once

#include <string>
#include <sstream>

namespace ren {

    class AbstractValue {
    public:
        virtual ~AbstractValue() {}
        virtual std::string asString() const = 0;
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

        std::string asString() const {
            std::ostringstream os;
            os << value;
            return os.str();
        }

        T value;
    };

}
