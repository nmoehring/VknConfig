cmake_minimum_required(VERSION 3.14)
project(VknConfigTests)

add_executable(VknVectorTests test_vknvector.cpp)

# Link your VknConfig library (replace VknConfigLib with your actual library target name)
# If VknCommon.hpp is header-only and included directly, you might not need to link a separate lib
# target_link_libraries(VknVectorTests PRIVATE VknConfigLib) # If VknConfig is a library

# Link Google Test
target_link_libraries(VknVectorTests PRIVATE GTest::gtest GTest::gtest_main)

# Add VknConfig include directory
target_include_directories(VknVectorTests PRIVATE ${CMAKE_SOURCE_DIR}/VknConfig/include)

include(GoogleTest)
gtest_discover_tests(VknVectorTests)