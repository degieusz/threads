#include <gtest/gtest.h>

// add test files here
#include "tests/test1.cpp"
int main(int argc, char **argv) {
::testing::InitGoogleTest( &argc, argv );
return RUN_ALL_TESTS();
}

