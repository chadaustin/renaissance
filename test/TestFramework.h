#ifndef REN_TEST_FRAMEWORK_H
#define REN_TEST_FRAMEWORK_H


#include <boost/test/auto_unit_test.hpp>


#define TEST(name) BOOST_AUTO_UNIT_TEST(name)
#define CHECK(x) BOOST_CHECK(x)
#define CHECK_EQUAL(x, y) BOOST_CHECK_EQUAL(x, y)


#endif
