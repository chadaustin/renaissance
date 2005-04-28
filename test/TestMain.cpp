#include <iostream>
#include <antlr/ANTLRException.hpp>
#include "TestFramework.h"
using namespace boost::unit_test;


static void ANTLRTranslator(const antlr::ANTLRException& e) {
    BOOST_ERROR("Caught ANTLRException: " + e.toString());
}


test_suite* init_unit_test_suite(int /*argc*/, char* /*argv*/ []) {
    register_exception_translator<antlr::ANTLRException>(&ANTLRTranslator);
    return ut_detail::auto_unit_test_suite();
}
