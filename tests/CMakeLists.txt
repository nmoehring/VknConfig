cmake_minimum_required(VERSION 3.22)
project(VknConfigTests)

set(TEST_SOURCES
    test_vknvector.cpp
    test_vknvectoriterator.cpp
    test_vknspace.cpp)

if(ANDROID)
    add_library(VknConfigUnitTests PRIVATE ${TEST_SOURCES})
else()
    add_executable(VknConfigUnitTests ${TEST_SOURCES})
endif()

# Link your VknConfig library (replace VknConfigLib with your actual library target name)
# If VknData.hpp is header-only and included directly, you might not need to link a separate lib
# target_link_libraries(VknVectorTests PRIVATE VknConfigLib) # If VknConfig is a library

# Link Google Test
target_link_libraries(VknConfigUnitTests PRIVATE VknConfig VknConfig_args GTest::gtest GTest::gtest_main)

# Add VknConfig include directory
target_include_directories(VknConfigUnitTests PRIVATE ${CMAKE_SOURCE_DIR}/VknConfig/include)

include(GoogleTest)