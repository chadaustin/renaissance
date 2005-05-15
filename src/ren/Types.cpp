#include <boost/noncopyable.hpp>
#include "Errors.h"
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


#define REN_PRIMITIVE_TYPES                                             \
        REN_PRIMITIVE_TYPE(FLOAT, "float")                              \
        REN_PRIMITIVE_TYPE(VEC2,  "vec2")                               \
        REN_PRIMITIVE_TYPE(VEC3,  "vec3")                               \
        REN_PRIMITIVE_TYPE(VEC4,  "vec4")                               \
                                                                        \
        REN_PRIMITIVE_TYPE(INT,   "int")                                \
        REN_PRIMITIVE_TYPE(VEC2I, "vec2i")                              \
        REN_PRIMITIVE_TYPE(VEC3I, "vec3i")                              \
        REN_PRIMITIVE_TYPE(VEC4I, "vec4i")                              \
                                                                        \
        REN_PRIMITIVE_TYPE(BOOL,  "bool")                               \
        REN_PRIMITIVE_TYPE(VEC2B, "vec2b")                              \
        REN_PRIMITIVE_TYPE(VEC3B, "vec3b")                              \
        REN_PRIMITIVE_TYPE(VEC4B, "vec4b")                              \
                                                                        \
        REN_PRIMITIVE_TYPE(MAT2, "mat2")                                \
        REN_PRIMITIVE_TYPE(MAT3, "mat3")                                \
        REN_PRIMITIVE_TYPE(MAT4, "mat4")                                \
                                                                        \
        REN_PRIMITIVE_TYPE(SAMPLER1D,       "sampler1D")                \
        REN_PRIMITIVE_TYPE(SAMPLER2D,       "sampler2D")                \
        REN_PRIMITIVE_TYPE(SAMPLER3D,       "sampler3D")                \
        REN_PRIMITIVE_TYPE(SAMPLERCUBE,     "samplerCube")              \
        REN_PRIMITIVE_TYPE(SAMPLER1DSHADOW, "sampler1DShadow")          \
        REN_PRIMITIVE_TYPE(SAMPLER2DSHADOW, "sampler2DShadow")


    enum PrimitiveTypeCode {

#define REN_PRIMITIVE_TYPE(code, name) _##code,
        REN_PRIMITIVE_TYPES
#undef REN_PRIMITIVE_TYPE

    };


    class PrimitiveType : public TypeObject {
    public:
        PrimitiveType(PrimitiveTypeCode code)
        : _code(code) {
        }

        const string getName() const {
            switch (_code) {

#define REN_PRIMITIVE_TYPE(code, name) case _##code: return name;
        REN_PRIMITIVE_TYPES
#undef REN_PRIMITIVE_TYPE
                default: assert(false); return "<unknown>";
            }
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


    Type getTypeFromString(const string& name) {
#define REN_PRIMITIVE_TYPE(code, n) if (name == n) return code;
        REN_PRIMITIVE_TYPES
#undef REN_PRIMITIVE_TYPE
            throw CompileError(name + ": unknown type");
    }


    Type getElementType(Type t) {
        if (t == FLOAT) return FLOAT;
        if (t == VEC2)  return FLOAT;
        if (t == VEC3)  return FLOAT;
        if (t == VEC4)  return FLOAT;

        if (t == INT)   return INT;
        if (t == VEC2I) return INT;
        if (t == VEC3I) return INT;
        if (t == VEC4I) return INT;

        if (t == BOOL)  return BOOL;
        if (t == VEC2B) return BOOL;
        if (t == VEC3B) return BOOL;
        if (t == VEC4B) return BOOL;

        return NullType;
    }


    unsigned getVectorLength(Type t) {
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

        return 0;
    }

    Type getVectorType(Type element, int length) {
        if (element == FLOAT) {
            if (length == 1) return FLOAT;
            if (length == 2) return VEC2;
            if (length == 3) return VEC3;
            if (length == 4) return VEC4;
        }
        if (element == INT) {
            if (length == 1) return INT;
            if (length == 2) return VEC2I;
            if (length == 3) return VEC3I;
            if (length == 4) return VEC4I;
        }
        if (element == BOOL) {
            if (length == 1) return BOOL;
            if (length == 2) return VEC2I;
            if (length == 3) return VEC3I;
            if (length == 4) return VEC4I;
        }
        return NullType;
    }



    const Type NullType(new NullTypeObject);

#define REN_PRIMITIVE_TYPE(code, name) const Type code(new PrimitiveType(_##code));
    REN_PRIMITIVE_TYPES
#undef REN_PRIMITIVE_TYPE

}
