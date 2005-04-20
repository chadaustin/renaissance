#ifndef REN_TYPES_H
#define REN_TYPES_H


#include <string>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>


namespace ren {

    using std::string;


    class TypeObject : public boost::noncopyable {
    public:
        // This virtual destructor isn't strictly necessary, because we use
        // boost::shared_ptr.  But gcc complains loudly.
        virtual ~TypeObject() { }

        virtual const string getName() const = 0;
        virtual bool operator==(const TypeObject& rhs) const = 0;

        bool operator!=(const TypeObject& rhs) const {
            return !(*this == rhs);
        }
    };
    typedef boost::shared_ptr<TypeObject> TypeObjectPtr;


    enum PrimitiveTypeCode {
        _FLOAT,
        _VEC2,
        _VEC3,
        _VEC4,

        _INT,
        _VEC2I,
        _VEC3I,
        _VEC4I,

        _BOOL,
        _VEC2B,
        _VEC3B,
        _VEC4B,

        _MAT2,
        _MAT3,
        _MAT4,

        _SAMPLER1D,
        _SAMPLER2D,
        _SAMPLER3D,
        _SAMPLERCUBE,
        _SAMPLER1DSHADOW,
        _SAMPLER2DSHADOW,
    };


    class PrimitiveType : public TypeObject {
    public:
        PrimitiveType(PrimitiveTypeCode code)
        : _code(code) {
        }

        const string getName() const;
        bool operator==(const TypeObject& rhs) const;

    private:
        PrimitiveTypeCode _code;
    };


    class TupleType : public TypeObject {
    public:
        TupleType() {
        }

        TupleType(TypeObjectPtr t1) {
            assert(t1);
            _elements.push_back(t1);
        }

        TupleType(TypeObjectPtr t1, TypeObjectPtr t2) {
            assert(t1);
            assert(t2);
            _elements.push_back(t1);
            _elements.push_back(t2);
        }

        TupleType(const std::vector<TypeObjectPtr>& elements)
        : _elements(elements) {
            for (size_t i = 0; i < _elements.size(); ++i) {
                assert(_elements[i]);
            }
        }

        const string getName() const;
        bool operator==(const TypeObject& rhs) const;

    private:
        std::vector<TypeObjectPtr> _elements;
    };


    class FunctionType : public TypeObject {
    public:
        FunctionType(TypeObjectPtr in, TypeObjectPtr out)
        : _in(in)
        , _out(out) {
            assert(in);
            assert(out);
        }

        const string getName() const;
        bool operator==(const TypeObject& rhs) const;

        TypeObjectPtr getInType()  const { return _in;  }
        TypeObjectPtr getOutType() const { return _out; }

    private:
        TypeObjectPtr _in;
        TypeObjectPtr _out;
    };


    /**
     * A value-semantics TypeObjects wrapper, so I don't go insane with
     * Java-ish code.
     */
    class Type {
    public:
        template<typename TypeType>
        Type(TypeType* object)
        : _object(object) {
            assert(_object);
        }

        template<typename TypeType>
        Type(boost::shared_ptr<TypeType> object)
        : _object(object) {
            assert(_object);
        }

        Type(const Type& rhs)
        : _object(rhs._object) {
        }

        TypeObjectPtr get() const {
            return _object;
        }

        const string getName() const {
            return _object->getName();
        }

        bool operator==(const Type& rhs) const {
            return *_object == *rhs._object;
        }

        bool operator!=(const Type& rhs) const {
            return !(*this == rhs);
        }

    private:
        TypeObjectPtr _object;
    };

    inline std::ostream& operator<<(std::ostream& os, const Type& type) {
        return os << type.getName();
    }


    inline Type makeTupleType(Type t1, Type t2) {
        return new TupleType(t1.get(), t2.get());
    }

    inline Type makeFunctionType(Type in, Type out) {
        return new FunctionType(in.get(), out.get());
    }


    static Type FLOAT(new PrimitiveType(_FLOAT));
    static Type VEC2 (new PrimitiveType(_VEC2));
    static Type VEC3 (new PrimitiveType(_VEC3));
    static Type VEC4 (new PrimitiveType(_VEC4));

    static Type INT  (new PrimitiveType(_INT));
    static Type VEC2I(new PrimitiveType(_VEC2I));
    static Type VEC3I(new PrimitiveType(_VEC3I));
    static Type VEC4I(new PrimitiveType(_VEC4I));

    static Type BOOL (new PrimitiveType(_BOOL));
    static Type VEC2B(new PrimitiveType(_VEC2B));
    static Type VEC3B(new PrimitiveType(_VEC3B));
    static Type VEC4B(new PrimitiveType(_VEC4B));

    static Type MAT2(new PrimitiveType(_MAT2));
    static Type MAT3(new PrimitiveType(_MAT3));
    static Type MAT4(new PrimitiveType(_MAT4));

    static Type SAMPLER1D      (new PrimitiveType(_SAMPLER1D));
    static Type SAMPLER2D      (new PrimitiveType(_SAMPLER2D));
    static Type SAMPLER3D      (new PrimitiveType(_SAMPLER3D));
    static Type SAMPLERCUBE    (new PrimitiveType(_SAMPLERCUBE));
    static Type SAMPLER1DSHADOW(new PrimitiveType(_SAMPLER1DSHADOW));
    static Type SAMPLER2DSHADOW(new PrimitiveType(_SAMPLER2DSHADOW));


    enum Frequency {
        CONSTANT,
        UNIFORM,
        VERTEX,
        FREQUENCY,
    };

}


#endif
