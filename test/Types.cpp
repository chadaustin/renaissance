#include "TestPrologue.h"


TEST(NullType) {
    CHECK_EQUAL(NullType, NullType);
    CHECK_EQUAL(NullType.getName(), "null");

    CHECK_EQUAL(NullType, NullType * NullType);

    Type t1 = NullType;
    Type t2 = NullType;
    CHECK(!(t1 < t2));
    CHECK(!(t2 < t1));
}


TEST(PrimitiveTypes) {
    CHECK_EQUAL(INT, INT);
    CHECK(INT != FLOAT);

    Type t1 = INT;
    Type t2 = FLOAT;
    CHECK(t1 < t2 || t2 < t1);
}


TEST(Tuples) {
    CHECK(INT == NullType * INT);
    CHECK(NullType * INT == INT);

    TypeList tl0;
    Type tuple0(makeTuple(tl0));
    CHECK_EQUAL(tuple0.getName(), "null");
    CHECK_EQUAL(tuple0, NullType);


    TypeList tl1;
    tl1.push_back(INT);
    Type tuple1(makeTuple(tl1));
    CHECK_EQUAL(tuple1.getName(), "int");
    CHECK_EQUAL(tuple1, INT);


    TypeList tl2;
    tl2.push_back(INT);
    tl2.push_back(FLOAT);
    Type tuple2(makeTuple(tl2));
    
    CHECK_EQUAL(tuple2.getName(), "(int, float)");
    CHECK_EQUAL(tuple2, INT * FLOAT);


    TypeList tl3;
    tl3.push_back(INT);
    tl3.push_back(FLOAT);
    tl3.push_back(BOOL);

    Type tuple3_list(makeTuple(tl3));
    Type tuple3_cons(INT * FLOAT * BOOL);

    CHECK_EQUAL(tuple3_list.getName(), "(int, float, bool)");
    CHECK_EQUAL(tuple3_cons.getName(), "(int, float, bool)");
    CHECK_EQUAL(tuple3_list, tuple3_cons);

    TypeList tl3_extract = asTuple(tuple3_list);
    CHECK_EQUAL(tl3_extract.size(), 3U);
    CHECK_EQUAL(tl3_extract[0], INT);
    CHECK_EQUAL(tl3_extract[1], FLOAT);
    CHECK_EQUAL(tl3_extract[2], BOOL);
}


TEST(Functions) {
    CHECK_EQUAL(NullType >> INT, INT);
    CHECK_EQUAL(INT >> NullType, INT);
    CHECK_EQUAL(INT >> FLOAT, INT >> FLOAT);

    Type t1(INT >> FLOAT);
    Function f1(asFunction(t1));
    CHECK_EQUAL(f1.in,  INT);
    CHECK_EQUAL(f1.out, FLOAT);

    Type t2(INT);
    Function f2(asFunction(t2));
    CHECK_EQUAL(f2.in,  NullType);
    CHECK_EQUAL(f2.out, INT);
}
