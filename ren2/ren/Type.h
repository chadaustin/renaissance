namespace ren {
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

}
