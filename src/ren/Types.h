#ifndef REN_TYPES_H
#define REN_TYPES_H


#include <vector>
#include "Base.h"


namespace ren {

    class TypeObject;
    REN_SHARED_PTR(TypeObject);

    /**
     * A value-semantics TypeObject wrapper, so I don't go insane with
     * Java-ish code.
     */
    class Type {
    public:
        Type(NotNull<TypeObject*> object);
        Type(NotNull<TypeObjectPtr> object);
        Type(const Type& rhs);

        const string getName() const;

        bool operator==(const Type& rhs) const;
        bool operator!=(const Type& rhs) const;
        bool operator<(const Type& rhs) const {
            // We're going to treat name as being the unique "canonical"
            // version of the type.
            return getName() < rhs.getName();
        }

        TypeObjectPtr get() const {
            return _object;
        }

    private:
        TypeObjectPtr _object;
    };
    typedef std::vector<Type> TypeList;

    inline std::ostream& operator<<(std::ostream& os, const Type& type) {
        return os << type.getName();
    }


    Type operator*(Type lhs, Type rhs);
    Type operator>>(Type lhs, Type rhs);


    Type makeTuple(const TypeList& list);
    TypeList asTuple(Type t);


    struct Function {
        Function(Type in_, Type out_)
        : in(in_)
        , out(out_) {
        }

        Type in;
        Type out;
    };
    Function asFunction(Type t);


    Type getTypeFromString(const string& name);

    Type getElementType(Type t);
    unsigned getVectorLength(Type t);
    Type getVectorType(Type element, unsigned length);
    unsigned getMatrixLength(Type t);
    Type getMatrixType(Type t, unsigned length);


    extern const Type NullType;

    extern const Type FLOAT;
    extern const Type VEC2;
    extern const Type VEC3;
    extern const Type VEC4;

    extern const Type INT;
    extern const Type VEC2I;
    extern const Type VEC3I;
    extern const Type VEC4I;

    extern const Type BOOL; 
    extern const Type VEC2B;
    extern const Type VEC3B;
    extern const Type VEC4B;

    extern const Type MAT2;
    extern const Type MAT3;
    extern const Type MAT4;

    extern const Type SAMPLER1D;
    extern const Type SAMPLER2D;
    extern const Type SAMPLER3D;
    extern const Type SAMPLERCUBE;
    extern const Type SAMPLER1DSHADOW;
    extern const Type SAMPLER2DSHADOW;


}


#endif
