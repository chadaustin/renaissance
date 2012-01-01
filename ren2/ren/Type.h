#pragma once

namespace ren {
    enum Frequency {
        CONSTANT,  // aka batch
        UNIFORM,   // aka batch
        ATTRIBUTE, // aka vertex
        //FRAGMENT,  // aka pixel
    };

    struct Type {
        enum ElementType {
            BOOL,
            INT,
            FLOAT,
        };

        static Type scalar(ElementType et) {
            return vector(et, 1);
        }

        static Type vector(ElementType et, unsigned dimensions) {
            return matrix(et, 1, dimensions);
        }
        
        static Type matrix(ElementType et, unsigned columns, unsigned rows) {
            // TODO: verify columns >= 1 && columns <= 4
            // TODO: verify rows >= 1 && rows <= 4
            Type rv;
            rv.is_array = 0;
            rv.array_length = 0;
            rv.columns = columns - 1;
            rv.rows = rows - 1;
            rv.element_type = et;
            return rv;
        }

        static Type array(Type v, unsigned length) {
            v.is_array = true;
            v.array_length = length;
            return v;
        }

        Type withoutArray() const {
            Type rv = *this;
            rv.is_array = 0;
            rv.array_length = 0;
            return rv;
        }

        ElementType element_type : 4;
        unsigned columns : 2; // implied + 1
        unsigned rows : 2; // implied + 1
        bool is_array : 1;
        unsigned array_length : 15;
    };

    const Type BOOL(Type::scalar(Type::BOOL));
    const Type BVEC2(Type::vector(Type::BOOL, 2));
    const Type BVEC3(Type::vector(Type::BOOL, 3));
    const Type BVEC4(Type::vector(Type::BOOL, 4));
    const Type INT(Type::scalar(Type::INT));
    const Type IVEC2(Type::vector(Type::INT, 2));
    const Type IVEC3(Type::vector(Type::INT, 3));
    const Type IVEC4(Type::vector(Type::INT, 4));
    const Type FLOAT(Type::scalar(Type::FLOAT));
    const Type VEC2(Type::vector(Type::FLOAT, 2));
    const Type VEC3(Type::vector(Type::FLOAT, 3));
    const Type VEC4(Type::vector(Type::FLOAT, 4));
    const Type MAT2(Type::matrix(Type::FLOAT, 2, 2));
    const Type MAT3(Type::matrix(Type::FLOAT, 3, 3));
    const Type MAT4(Type::matrix(Type::FLOAT, 4, 4));

    template<typename T> struct TypeMap;
#define REN_TYPEMAP(c, g) template<> struct TypeMap<c> { static Type get() { return g; } };
    REN_TYPEMAP(bool, BOOL);
    REN_TYPEMAP(class bvec2, BVEC2);
    REN_TYPEMAP(class bvec3, BVEC3);
    REN_TYPEMAP(class bvec4, BVEC4);
    REN_TYPEMAP(int, INT);
    REN_TYPEMAP(class ivec2, IVEC2);
    REN_TYPEMAP(class ivec3, IVEC3);
    REN_TYPEMAP(class ivec4, IVEC4);
    REN_TYPEMAP(float, FLOAT);
    REN_TYPEMAP(class vec2, VEC2);
    REN_TYPEMAP(class vec3, VEC3);
    REN_TYPEMAP(class vec4, VEC4);
    REN_TYPEMAP(class mat2, MAT2);
    REN_TYPEMAP(class mat3, MAT3);
    REN_TYPEMAP(class mat4, MAT4);
#undef REN_TYPEMAP

    template<typename T>
    Type getType() {
        return TypeMap<T>::get();
    }

    template<typename T> struct NativeToRen {
        typedef T type;
    };
    template<> struct NativeToRen<bool> {
        typedef class bool_ type;
    };
    template<> struct NativeToRen<int> {
        typedef class int_ type;
    };
    template<> struct NativeToRen<float> {
        typedef class float_ type;
    };

}
