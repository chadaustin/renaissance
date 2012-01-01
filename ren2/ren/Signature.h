#pragma once

namespace ren {

    struct Signature {
        Type result;
        std::vector<Type> arguments;
    };

    template<typename T>
    struct SignatureType;

    template<typename T>
    struct SignatureType<T()> {
        static Signature build() {
            Signature rv;
            rv.result = getType<T>();
            return rv;
        }
    };

    template<typename T, typename A>
    struct SignatureType<T(A)> {
        static Signature build() {
            Signature rv;
            rv.result = getType<T>();
            rv.arguments.push_back(getType<A>());
            return rv;
        }
    };

    template<typename T, typename A, typename B>
    struct SignatureType<T(A,B)> {
        static Signature build() {
            Signature rv;
            rv.result = getType<T>();
            rv.arguments.push_back(getType<A>());
            rv.arguments.push_back(getType<B>());
            return rv;
        }
    };

    template<typename T, typename A, typename B, typename C>
    struct SignatureType<T(A,B,C)> {
        static Signature build() {
            Signature rv;
            rv.result = getType<T>();
            rv.arguments.push_back(getType<A>());
            rv.arguments.push_back(getType<B>());
            rv.arguments.push_back(getType<C>());
            return rv;
        }
    };

    template<typename T, typename A, typename B, typename C, typename D>
    struct SignatureType<T(A,B,C,D)> {
        static Signature build() {
            Signature rv;
            rv.result = getType<T>();
            rv.arguments.push_back(getType<A>());
            rv.arguments.push_back(getType<B>());
            rv.arguments.push_back(getType<C>());
            rv.arguments.push_back(getType<D>());
            return rv;
        }
    };

    template<typename T>
    Signature getSignature() {
        return SignatureType<T>::build();
    }
}
