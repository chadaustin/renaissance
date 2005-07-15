#ifndef REN_VALUE_H
#define REN_VALUE_H


#include <algorithm>
#include <sstream>
#include "Types.h"


namespace ren {

    class Value;
    REN_SHARED_PTR(Value);


    inline int getArity(Type t) {
        if (t == FLOAT) return 1;
        if (t == VEC2)  return 2;
        if (t == VEC3)  return 3;
        if (t == VEC4)  return 4;

        if (t == INT)   return 1;
        if (t == VEC2I) return 2;
        if (t == VEC3I) return 3;
        if (t == VEC4I) return 4;

        if (t == BOOL)  return 1; 
        if (t == VEC2B) return 2;
        if (t == VEC3B) return 3;
        if (t == VEC4B) return 4;

        if (t == MAT2)  return 4;
        if (t == MAT3)  return 9;
        if (t == MAT4)  return 16;

        if (t == SAMPLER1D)       return 1;
        if (t == SAMPLER2D)       return 1;
        if (t == SAMPLER3D)       return 1;
        if (t == SAMPLERCUBE)     return 1;
        if (t == SAMPLER1DSHADOW) return 1;
        if (t == SAMPLER2DSHADOW) return 1;

        assert(!"Compound type has no arity");
        return 0;
    }


    template<typename T>
    struct TypeChecker;

    template<>
    struct TypeChecker<float> {
        static void check(Type t) {
            assert(t == FLOAT || t == VEC2 || t == VEC3 || t == VEC4 ||
                   t == MAT2 || t == MAT3 || t == MAT4);
        }
    };

    template<>
    struct TypeChecker<int> {
        static void check(Type t) {
            assert(t == INT || t == VEC2I || t == VEC3I || t == VEC4I ||
                   t == SAMPLER1D || t == SAMPLER2D || t == SAMPLER3D ||
                   t == SAMPLERCUBE || t == SAMPLER1DSHADOW ||
                   t == SAMPLER2DSHADOW);
        }
    };

    template<>
    struct TypeChecker<bool> {
        static void check(Type t) {
            assert(t == BOOL || t == VEC2B || t == VEC3B || t == VEC4B);
        }
    };


    class Value {
    public:
        static ValuePtr create(Type type) {
            if (type == FLOAT) return create<float>(type, 0);
            if (type == VEC2)  return create<float>(type, 0);
            if (type == VEC3)  return create<float>(type, 0);
            if (type == VEC4)  return create<float>(type, 0);
            if (type == MAT2)  return create<float>(type, 0);
            if (type == MAT3)  return create<float>(type, 0);
            if (type == MAT4)  return create<float>(type, 0);

            if (type == INT)   return create<int>(type, 0);
            if (type == VEC2I) return create<int>(type, 0);
            if (type == VEC3I) return create<int>(type, 0);
            if (type == VEC4I) return create<int>(type, 0);

            if (type == BOOL)  return create<bool>(type, 0);
            if (type == VEC2B) return create<bool>(type, 0);
            if (type == VEC3B) return create<bool>(type, 0);
            if (type == VEC4B) return create<bool>(type, 0);

            if (type == SAMPLER1D)       return create<int>(type, 0);
            if (type == SAMPLER2D)       return create<int>(type, 0);
            if (type == SAMPLER3D)       return create<int>(type, 0);
            if (type == SAMPLERCUBE)     return create<int>(type, 0);
            if (type == SAMPLER1DSHADOW) return create<int>(type, 0);
            if (type == SAMPLER2DSHADOW) return create<int>(type, 0);
          
            assert(!"Invalid Value type");
            return ValuePtr();
        }

        template<typename T>
        static ValuePtr create(Type type, const T* data) {
            return ValuePtr(new Value(type, data));
        }

        template<typename T>
        Value(Type type, const T* data)
        : _type(type) {
            TypeChecker<T>::check(type);
            if (!data) {
                static T empty[maxArity];
                data = empty;
            }
            std::copy(data, data + getArity(type), getStorage<T>());
        }

        bool asBool() const {
            assert(_type == BOOL);
            return _b[0];
        }

        const float* asFloatVec() const { return _f; }
        const int*   asIntVec()   const { return _i; }
        const bool*  asBoolVec()  const { return _b; }

        int asInt() const { return *asIntVec(); }

        string asString() const {
            Type elementType = getElementType(_type);
            int arity = getArity(_type);
            if (elementType == FLOAT) {
                return _type.getName() + "(" +
                    asString(asFloatVec(), arity) + ")";
            }
            if (elementType == INT) {
                return _type.getName() + "(" +
                    asString(asIntVec(), arity) + ")";
            }
            if (elementType == BOOL) {
                return _type.getName() + "(" +
                    asString(asBoolVec(), arity) + ")";
            }
            assert(!"Cannot convert unknown type to expression.");
            return "Cannot convert unknown type to expression.";
        }

        bool operator<(const Value& rhs) const {
            assert(_type == rhs._type);
            Type elementType = getElementType(_type);
            int arity = getArity(_type);
            if (elementType == FLOAT) {
                return lessThan(arity, asFloatVec(), rhs.asFloatVec());
            }
            if (elementType == INT) {
                return lessThan(arity, asIntVec(), rhs.asIntVec());
            }
            if (elementType == BOOL) {
                return lessThan(arity, asBoolVec(), rhs.asBoolVec());
            }
            assert(!"Unknown element type -- can't compare.");
            return false;
        }

    private:
        static const int maxArity = 16;

        template<typename T>
        string asString(const T* data, int arity) const {
            string rv;
            for (const T* p = data; p != data + arity; ++p) {
                if (p != data) {
                    rv += ", ";
                }
                rv += asString(*p);
            }
            return rv;
        }

        static string asString(float f) {
            char buf[80];
            sprintf(buf, "%.1f", f);
            return buf;
        }

        static string asString(int i) {
            std::ostringstream os;
            os << i;
            return os.str();
        }

        static string asString(bool b) {
            return b ? "true" : "false";
        }

        template<typename T>
        static bool lessThan(int arity, const T* lhs, const T* rhs) {
            for (int i = 0; i < arity; ++i) {
                if (lhs[i] < rhs[i]) return true;
                if (rhs[i] < lhs[i]) return false;
            }
            return false;
        }


        template<typename T>
        struct Type2Type {
            typedef T Type;
        };

        // Can't specialize member templates, so...
        template<typename T>
        T* getStorage() {
            return getStorage(Type2Type<T>());
        }

        bool*  getStorage(Type2Type<bool>)  { return _b; }
        int*   getStorage(Type2Type<int>)   { return _i; }
        float* getStorage(Type2Type<float>) { return _f; }


        Type _type;
        union {
            bool  _b[maxArity];
            int   _i[maxArity];
            float _f[maxArity];
        };
    };


    static ValuePtr NullValue;

};


#endif
