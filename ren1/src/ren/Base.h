#ifndef REN_BASE_H
#define REN_BASE_H


#include <string>
#include <boost/shared_ptr.hpp>


#define REN_SHARED_PTR(C) typedef boost::shared_ptr<C> C##Ptr

#define REN_DYNAMIC_CAST(name, type, object)    \
    type name = dynamic_cast<type>(object)

#define REN_DYNAMIC_CAST_PTR(name, type, object)        \
    boost::shared_ptr<type> name = boost::dynamic_pointer_cast<type>(object)


namespace ren {

    using std::string;


    template<typename T>
    class NotNull {
    public:
        NotNull(T object)
        : _object(object) {
            assert(object);
        }

        operator T() const {
            return _object;
        }

        T get() const {
            return _object;
        }

    private:
        T _object;
    };

}


#endif
