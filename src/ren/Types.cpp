#include <boost/noncopyable.hpp>
#include "Types.h"


namespace ren {

    class TypeObject : public boost::noncopyable {
    public:
        // This virtual destructor isn't strictly necessary, because we use
        // boost::shared_ptr.  But gcc complains.  Loudly.
        virtual ~TypeObject() { }

        virtual const string getName() const = 0;
        virtual bool operator==(const TypeObject& rhs) const = 0;

        bool operator!=(const TypeObject& rhs) const {
            return !(*this == rhs);
        }
    };
    typedef std::vector<TypeObjectPtr> TypeObjectList;


    class NullTypeObject : public TypeObject {
        const string getName() const {
            return "null";
        }

        bool operator==(const TypeObject& rhs) const {
            return dynamic_cast<const NullTypeObject*>(&rhs);
        }
    };
    REN_SHARED_PTR(NullTypeObject);


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

        const string getName() const {
#define REN_NAME_CASE(type, name) case _##type: return (name)
            switch (_code) {
                REN_NAME_CASE(FLOAT, "float");
                REN_NAME_CASE(VEC2,  "vec2");
                REN_NAME_CASE(VEC3,  "vec3");
                REN_NAME_CASE(VEC4,  "vec4");

                REN_NAME_CASE(INT,   "int");
                REN_NAME_CASE(VEC2I, "vec2i");
                REN_NAME_CASE(VEC3I, "vec3i");
                REN_NAME_CASE(VEC4I, "vec4i");

                REN_NAME_CASE(BOOL,  "bool");
                REN_NAME_CASE(VEC2B, "vec2b");
                REN_NAME_CASE(VEC3B, "vec3b");
                REN_NAME_CASE(VEC4B, "vec4b");

                REN_NAME_CASE(MAT2, "mat2");
                REN_NAME_CASE(MAT3, "mat3");
                REN_NAME_CASE(MAT4, "mat4");

                REN_NAME_CASE(SAMPLER1D,       "sampler1D");
                REN_NAME_CASE(SAMPLER2D,       "sampler2D");
                REN_NAME_CASE(SAMPLER3D,       "sampler3D");
                REN_NAME_CASE(SAMPLERCUBE,     "samplerCube");
                REN_NAME_CASE(SAMPLER1DSHADOW, "sampler1DShadow");
                REN_NAME_CASE(SAMPLER2DSHADOW, "sampler2DShadow");
                default: assert(false); return "<unknown>";
            }
#undef REN_NAME_CASE
        }

        bool operator==(const TypeObject& rhs) const {
            if (REN_DYNAMIC_CAST(p, const PrimitiveType*, &rhs)) {
                return _code == p->_code;
            } else {
                return false;
            }
        }

    private:
        PrimitiveTypeCode _code;
    };


    class TupleTypeObject : public TypeObject {
    public:
        TupleTypeObject(const TypeObjectList& elements)
        : _elements(elements) {
            assert(elements.size() >= 2);
            for (size_t i = 0; i < _elements.size(); ++i) {
                assert(_elements[i]);
            }
        }

        const string getName() const {
            string result = "(";
            for (size_t i = 0; i < _elements.size(); ++i) {
                if (i != 0) {
                    result += ", ";
                }
                result += _elements[i]->getName();
            }
            result += ")";
            return result;
        }

        bool operator==(const TypeObject& rhs) const {
            if (REN_DYNAMIC_CAST(p, const TupleTypeObject*, &rhs)) {
                if (_elements.size() != p->_elements.size()) {
                    return false;
                }
                for (size_t i = 0; i < _elements.size(); ++i) {
                    if (*(_elements[i]) != *(p->_elements[i])) {
                        return false;
                    }
                }
                return true;
            } else {
                return false;
            }
        }

        const TypeObjectList& getElements() const {
            return _elements;
        }

    private:
        TypeObjectList _elements;
    };
    REN_SHARED_PTR(TupleTypeObject);


    class FunctionTypeObject : public TypeObject {
    public:
        FunctionTypeObject(NotNull<TypeObjectPtr> in, NotNull<TypeObjectPtr> out)
        : _in(in)
        , _out(out) {
        }

        const string getName() const {
            return "(" + _in->getName() + " -> " + _out->getName() + ")";
        }

        bool operator==(const TypeObject& rhs) const {
            if (REN_DYNAMIC_CAST(p, const FunctionTypeObject*, &rhs)) {
                return *_in == *p->_in && *_out == *p->_out;
            } else {
                return false;
            }
        }

        TypeObjectPtr getInType()  const { return _in;  }
        TypeObjectPtr getOutType() const { return _out; }

    private:
        TypeObjectPtr _in;
        TypeObjectPtr _out;
    };
    REN_SHARED_PTR(FunctionTypeObject);


    Type::Type(NotNull<TypeObject*> object)
    : _object(object.get()) {
    }

    Type::Type(NotNull<TypeObjectPtr> object)
    : _object(object.get()) {
    }

    Type::Type(const Type& rhs)
    : _object(rhs.get()) {
    }

    const string Type::getName() const {
        return _object->getName();
    }

    bool Type::operator==(const Type& rhs) const {
        return *_object == *rhs._object;
    }

    bool Type::operator!=(const Type& rhs) const {
        return !(*this == rhs);
    }


    Type operator*(Type lhs, Type rhs) {
        REN_DYNAMIC_CAST_PTR(lhs_n, NullTypeObject, lhs.get());
        REN_DYNAMIC_CAST_PTR(rhs_n, NullTypeObject, rhs.get());

        if (lhs_n) {
            return rhs;
        } else if (rhs_n) {
            return lhs;
        }

        REN_DYNAMIC_CAST_PTR(lhs_t, TupleTypeObject, lhs.get());
        REN_DYNAMIC_CAST_PTR(rhs_t, TupleTypeObject, rhs.get());

        if (lhs_t && rhs_t) {

            TypeObjectList newElements(lhs_t->getElements());
            newElements.insert(newElements.end(),
                               rhs_t->getElements().begin(),
                               rhs_t->getElements().end());
            return Type(new TupleTypeObject(newElements));

        } else if (lhs_t) {

            TypeObjectList newElements(lhs_t->getElements());
            newElements.push_back(rhs.get());
            return Type(new TupleTypeObject(newElements));

        } else if (rhs_t) {

            TypeObjectList newElements(1, lhs.get());
            newElements.insert(newElements.end(),
                               rhs_t->getElements().begin(),
                               rhs_t->getElements().end());
            return Type(new TupleTypeObject(newElements));

        } else {

            TypeObjectList newElements(2);
            newElements[0] = lhs.get();
            newElements[1] = rhs.get();
            return Type(new TupleTypeObject(newElements));

        }
    }


    Type operator>>(Type lhs, Type rhs) {
        REN_DYNAMIC_CAST_PTR(lhs_n, NullTypeObject, lhs.get());
        REN_DYNAMIC_CAST_PTR(rhs_n, NullTypeObject, rhs.get());

        if (lhs_n) {
            return rhs;
        } else if (rhs_n) {
            return lhs;
        }

        return Type(TypeObjectPtr(new FunctionTypeObject(
                                      lhs.get(),
                                      rhs.get())));
    }


    Type makeTuple(const TypeList& list) {
        Type rv = NullType;
        for (size_t i = 0; i < list.size(); ++i) {
            rv = rv * list[i];
        }
        return rv;
    }


    TypeList asTuple(Type t) {
        if (dynamic_cast<NullTypeObject*>(t.get().get())) {
            return TypeList();
        } else if (REN_DYNAMIC_CAST(p, TupleTypeObject*, t.get().get())) {
            TypeList rv;
            for (size_t i = 0; i < p->getElements().size(); ++i) {
                rv.push_back(Type(p->getElements()[i]));
            }
            return rv;
        } else {
            return TypeList(1, t);
        }
    }


    Function asFunction(Type t) {
        if (REN_DYNAMIC_CAST(p, FunctionTypeObject*, t.get().get())) {
            return Function(Type(p->getInType()), Type(p->getOutType()));
        } else {
            return Function(NullType, t);
        }
    }


    const Type NullType(new NullTypeObject);

    const Type FLOAT(new PrimitiveType(_FLOAT));
    const Type VEC2 (new PrimitiveType(_VEC2));
    const Type VEC3 (new PrimitiveType(_VEC3));
    const Type VEC4 (new PrimitiveType(_VEC4));

    const Type INT  (new PrimitiveType(_INT));
    const Type VEC2I(new PrimitiveType(_VEC2I));
    const Type VEC3I(new PrimitiveType(_VEC3I));
    const Type VEC4I(new PrimitiveType(_VEC4I));

    const Type BOOL (new PrimitiveType(_BOOL));
    const Type VEC2B(new PrimitiveType(_VEC2B));
    const Type VEC3B(new PrimitiveType(_VEC3B));
    const Type VEC4B(new PrimitiveType(_VEC4B));

    const Type MAT2(new PrimitiveType(_MAT2));
    const Type MAT3(new PrimitiveType(_MAT3));
    const Type MAT4(new PrimitiveType(_MAT4));

    const Type SAMPLER1D      (new PrimitiveType(_SAMPLER1D));
    const Type SAMPLER2D      (new PrimitiveType(_SAMPLER2D));
    const Type SAMPLER3D      (new PrimitiveType(_SAMPLER3D));
    const Type SAMPLERCUBE    (new PrimitiveType(_SAMPLERCUBE));
    const Type SAMPLER1DSHADOW(new PrimitiveType(_SAMPLER1DSHADOW));
    const Type SAMPLER2DSHADOW(new PrimitiveType(_SAMPLER2DSHADOW));

}
